/*
 * sad.c, Copyright (c) Alessandro Rubini, 1998
 */

#ifndef __KERNEL__
#  define __KERNEL__
#endif
#ifndef MODULE
#  define MODULE
#endif

#undef PDEBUG             /* undef it, just in case */
#ifdef SAD_DEBUG
#  ifdef __KERNEL__
     /* This one if debugging is on, and kernel space */
#    define PDEBUG(fmt, args...) printk( KERN_DEBUG "sad: " fmt, ## args)
#  else
     /* This one for user space */
#    define PDEBUG(fmt, args...) fprintf(stderr, fmt, ## args)
#  endif
#else
#  define PDEBUG(fmt, args...) /* not debugging: nothing */
#endif

#undef PDEBUGG
#define PDEBUGG(fmt, args...) /* nothing: it's a placeholder */


#include <linux/module.h>

#include <linux/sched.h>
#include <linux/kernel.h> /* printk() */
#include <linux/fs.h>     /* everything... */
#include <linux/errno.h>  /* error codes */
#include <linux/malloc.h>
#include <linux/mm.h>
#include <linux/ioport.h>
#include <linux/interrupt.h>

#include <asm/io.h>
#include <asm/segment.h>

#include "sysdep-2.1.h"

int sad_major = 0;
int sad_base = 0x378; /* intel: default to lp0 */
int sad_irq = -1;


/*=====================================================================*/
/* managing data buffers */

#define IBUFFER_ORDER 0 /* one page */
#define OBUFFER_ORDER 0 /* one page, too (but you'd need to increase it) */

#define IBUFFER_MASK ((1 << (PAGE_SHIFT+IBUFFER_ORDER)) -1)
#define OBUFFER_MASK ((1 << (PAGE_SHIFT+OBUFFER_ORDER)) -1)

#define IBUFFER_SIZE (PAGE_SIZE << IBUFFER_ORDER)
#define OBUFFER_SIZE (PAGE_SIZE << OBUFFER_ORDER)

static unsigned long ibuffer = 0, obuffer = 0;

/* reading data: irq writes at head */
static unsigned long volatile ihead, itail;
#define IBUFFER_EMPTY (ihead==itail)
#define IBUFFER_FULL (((itail-ihead)&IBUFFER_MASK)==1)

/* writing data: irq reads from tail */
static unsigned long volatile ohead, otail;
#define OBUFFER_EMPTY (ohead==otail)
#define OBUFFER_FREE ((otail-ohead-1)&OBUFFER_MASK)
#define OBUFFER_FULL (!OBUFFER_FREE)
/* #define OBUFFER_FULL (((otail-ohead)&OBUFFER_MASK)==1) */

#define OBUFFER_THRESHOLD 1024 /* don't awake unless at least 1k is free */

static struct wait_queue *inq, *outq, *closeq;

#define  SAD_IRQON()   (outb(0x10,sad_base+2))
#define  SAD_IRQOFF()  (outb(0x00,sad_base+2))

/*=====================================================================*/
/* The actual hw mechanism */

static inline u8 convert(u8 byte) 
{
    u8 b = byte & 0x80 ? byte&0x7f : 0xff^byte;
    return ((b>>2)&0x3f) | b<<6;
}

#define OUTBYTE(b)  outb(convert(b),sad_base)

/*=====================================================================*/
/* open and close */

int sad_open (struct inode *inode, struct file *filp)
{
    SAD_IRQON();
    MOD_INC_USE_COUNT;
    return 0;
}

release_t sad_release (struct inode *inode, struct file *filp)
{
    unsigned long flags;
    /*
     * If minor is 0, flush data to speaker before closing.
     * Otherwise, forget pending data.
     */
    
    if (MINOR(inode->i_rdev)==0)
	interruptible_sleep_on(&closeq);
    else {
	save_flags(flags);
	cli(); ohead=otail;
	restore_flags(flags);
    }

    MOD_DEC_USE_COUNT;
    if (!MOD_IN_USE)
        SAD_IRQOFF();
    release_return(0);
}



#ifdef __USE_OLD_SELECT__
int sad_poll (struct inode *inode, struct file *filp,
                  int mode, select_table *table)
{
    if (mode==SEL_IN) {
        if (!IBUFFER_EMPTY) return 1; /* readable */
        select_wait (&inq, table);
        return 0;
    }
    if (mode==SEL_OUT) {
        if (!OBUFFER_FULL) return 1; /* writable */
        select_wait (&outq, table);
        return 0;
    }
    return 0; /* not-exceptionable */
}
#else
unsigned int sad_poll (struct file *filp, poll_table *table)
{
    int ret=0;

    poll_wait(&inq,  table);
    poll_wait(&outq, table);
    if (!IBUFFER_EMPTY)
        ret |= POLLIN | POLLRDNORM;
    if (!OBUFFER_FULL)
        ret |= POLLOUT | POLLWRNORM;
    return ret;
}
#endif

/*=====================================================================*/
/* read: returns interrupt timestamps */

read_write_t sad_read (struct inode *inode, struct file *filp,
                char *buf, count_t count)
{
    int count0;

    while (IBUFFER_EMPTY) {
        if (filp->f_flags & O_NONBLOCK)
            return -EAGAIN;
        interruptible_sleep_on(&inq);
        if (current->signal & ~current->blocked) /* a signal arrived */
          return -ERESTARTSYS; /* tell the fs layer to handle it */
        /* else, loop */
    }
    /* count0 is the number of readable data bytes */
    count0 = ihead - itail;
    if (count0 < 0) /* wrapped */
        count0 = ibuffer + IBUFFER_SIZE - itail;
    if (count0 < count) count = count0;

    copy_to_user(buf, (char *)itail, count);
    itail += count;
    if (itail == ibuffer + IBUFFER_SIZE)
        itail = ibuffer;
    return count;
}

/*=====================================================================*/
/* write: send out one byte each time we get an interrupt */

read_write_t sad_write (struct inode *inode, struct file *filp,
                const char *buf, count_t count)
{
    int count0;

    PDEBUGG("write %4li (R %04lx,%04lx) (W %04lx,%04lx)\n",count,
	   ihead&IBUFFER_MASK,itail&IBUFFER_MASK,
	   ohead&OBUFFER_MASK,otail&OBUFFER_MASK);
    while (OBUFFER_FREE < OBUFFER_THRESHOLD) {
        if (filp->f_flags & O_NONBLOCK)
            return -EAGAIN;
        interruptible_sleep_on(&outq);
        if (current->signal & ~current->blocked) /* a signal arrived */
          return -ERESTARTSYS; /* tell the fs layer to handle it */
        /* else, loop */
    }
    /*
     * Written data is placed at ohead, and output proceeds from otail
     * onwards. Free apace is therefore tail-head-1, or end-head;
     */
    if (count > OBUFFER_FREE) count = OBUFFER_FREE;
    /* count0 is the number of writable data bytes */
    count0 = otail - ohead -1;
    if (count0 < 0) /* wrapped */
        count0 = obuffer + OBUFFER_SIZE - ohead;
    if (count0 < count) count = count0;

    copy_from_user((char *)ohead, buf, count);
    ohead += count;
    if (ohead == obuffer + OBUFFER_SIZE)
        ohead = obuffer;
    PDEBUGG("written %li\n",count);
    return count;
}

/*=====================================================================*/
lseek_t my_lseek(struct inode *, struct file *, lseek_off_t, int)
{ return -ESPIPE; }

/*=====================================================================*/

struct file_operations sad_fops = {
    sad_lseek,
    sad_read,
    sad_write,
    NULL,          /* sad_readdir */
    sad_poll,
    NULL,          /* sad_ioctl */
    NULL,          /* sad_mmap */
    sad_open,
    sad_release,
    NULL,          /* sad_fsync */
    NULL,          /* sad_fasync */
                   /* nothing more, fill with NULLs */
};

/*=====================================================================*/

void sad_interrupt(int irq, void *dev_id, struct pt_regs *regs)
{
    static struct timeval tv, tv_last;
    unsigned long diff=0;

    if (!OBUFFER_EMPTY) { /* send out one byte */
        OUTBYTE(*((u8 *)otail++));
        if (otail == obuffer + OBUFFER_SIZE) /* wrap */
            otail = obuffer;
        if (OBUFFER_FREE > OBUFFER_THRESHOLD)
            wake_up_interruptible(&outq);
        return;
    }
    wake_up_interruptible(&closeq);

    /* else, pass interrupts times to the reader */
    do_gettimeofday(&tv);
    diff = (tv.tv_sec  - tv_last.tv_sec) * 1000000 +
           (tv.tv_usec - tv_last.tv_usec);
    tv_last = tv;

    /* Write a 16 byte record. Assume IBUFFER_SIZE is a multiple of 16 */
    ihead += sprintf((char *)ihead,"%15u\n", (int)diff);
    if (ihead == ibuffer + IBUFFER_SIZE)
        ihead = ibuffer; /* wrap */

    wake_up_interruptible(&inq); /* awake any reading process */
}

/*=====================================================================*/

#if 0
void sad_kernelprobe(void)
{
    int count = 0;
    do {
        unsigned long mask;

        mask = probe_irq_on();
        outb_p(0x10,sad_base+2); /* enable reporting */
        outb_p(0x00,sad_base);   /* clear the bit */
        outb_p(0xFF,sad_base);   /* set the bit: interrupt! */
        outb_p(0x00,sad_base+2); /* disable reporting */
        sad_irq = probe_irq_off(mask);

        if (sad_irq == 0) { /* none of them? */
            printk(KERN_INFO "sad: no irq reported by probe\n");
            sad_irq = -1;
        }
        /*
         * if more than one line has been activated, the result is
         * negative. We should service the interrupt (no need for lpt port)
         * and loop over again. Loop at most five times, then give up
         */
    } while (sad_irq < 0 && count++ < 5);
    if (sad_irq < 0)
        printk("sad: probe failed %i times, giving up\n", count);
}
#endif

/*=====================================================================*/

void cleanup_module(void)
{
    if (sad_irq >= 0) free_irq(sad_irq, NULL);
        
    if (sad_major > 0) unregister_chrdev(sad_major, "sad");
    if (ibuffer) {
	free_pages(ibuffer,IBUFFER_ORDER);
	ibuffer=0;
    }
    if (obuffer) {
	free_pages(obuffer,OBUFFER_ORDER);
	obuffer=0;
    }
    release_region(sad_base,4);
}
/*=====================================================================*/

int init_module(void)
{
    int result = check_region(sad_base,4);

    if (result) {
        printk(KERN_INFO "sad: can't get I/O address 0x%x\n",sad_base);
        return result;
    }
    request_region(sad_base,4,"sad");

    ibuffer = __get_free_pages(GFP_KERNEL,IBUFFER_ORDER,0);
    ihead = itail = ibuffer;
    obuffer = __get_free_pages(GFP_KERNEL,OBUFFER_ORDER,0);
    ohead = otail = obuffer;

    if (!ibuffer || !obuffer) { /* allocation failed */
        cleanup_module(); /* use your own function */
        return -ENOMEM;
    }

    result = register_chrdev(sad_major, "sad", &sad_fops);
    if (result < 0) {
        cleanup_module(); /* use your own function */
        return result;
    }
    if (sad_major == 0) sad_major = result; /* dynamic */

    if (sad_irq < 0) /* not yet specified: force the default on */
        switch(sad_base) {
          case 0x378: sad_irq = 7; break;
          case 0x278: sad_irq = 2; break;
          case 0x3bc: sad_irq = 5; break;
        }
    /* sad_kernelprobe(); */

    result = request_irq(sad_irq, sad_interrupt,
                         SA_INTERRUPT, "sad", NULL);
    if (result) {
	sad_irq=-1;
        cleanup_module(); /* use your own function */
        return result;
    }
    
    return 0;
}

