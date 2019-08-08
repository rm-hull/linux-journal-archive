/*
 * Copyright (C) 1998 Alessandro Rubini (rubini@linux.it)
 * 
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <linux/module.h>

#include <linux/sched.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>

#include <asm/segment.h>

static struct miscdevice kiss_dev;

/* =================================== pointers to private kernel stuff */

#include <linux/../../drivers/char/kbd_kern.h>

unsigned char (*getledstateP)(void);
void (*setledstateP)(struct kbd_struct *kbd, unsigned int led);
/* the real kbd_table is an array, not a pointer. So a ptr is good here */
struct kbd_struct *kbd_tableP;
int *fg_consoleP;


/* =================================== a simple delayer */

static void delay_tenths(int tenths)
{
    unsigned long j = jiffies;

    /* delay tenths tenths of a second */
    j+= tenths * HZ / 10;
    while (j > jiffies)
        schedule();
}

/* =================================== open and close */

int kiss_open (struct inode *inode, struct file *filp)
{
    MOD_INC_USE_COUNT;
    return 0;
}

void kiss_close (struct inode *inode, struct file *filp)
{
    MOD_DEC_USE_COUNT;
}

/* =================================== read and write */

/*
 * The read() method just returns the current status of the leds.
 * Try "while true; do dd bs=1 if=/dev/kiss count=1; sleep 1; done"
 */
int kiss_read(struct inode *inode, struct file *filp,
              char *buf, int count)
{
    int done=0;
    /* the user space address is known to be good */

    while (done++ < count)
        put_user(getledstateP(), buf++);
    
    return count;
}

/*
 * Write understands a few textual commands
 */ 
int kiss_write(struct inode *inode, struct file *filp,
               const char *buf, int count)
{
    int done=0;
    unsigned char cmd;

    unsigned char leds = getledstateP();
    
    while (done++ < count) {
        switch(cmd = get_user(buf++)) {
          case 's': leds &= ~1; break;
          case 'n': leds &= ~2; break;
          case 'c': leds &= ~4; break;
          case 'S': leds |= 1; break;
          case 'N': leds |= 2; break;
          case 'C': leds |= 4; break;

          case '0': case '1': case '2': case '3':
          case '4': case '5': case '6': case '7': leds = cmd-'0'; break;

          case 't': delay_tenths(1); break;
          case 'T': delay_tenths(10); break;

          case 'f':
            setledstateP(kbd_tableP + *fg_consoleP, 0);
            delay_tenths(1);
            setledstateP(kbd_tableP + *fg_consoleP, 7); /* one */
            delay_tenths(1);
            setledstateP(kbd_tableP + *fg_consoleP, 0);
            delay_tenths(1);
            break;

          case 'F':
            setledstateP(kbd_tableP + *fg_consoleP, 0);
            delay_tenths(1);
            setledstateP(kbd_tableP + *fg_consoleP, 7); /* one */
            delay_tenths(1);
            setledstateP(kbd_tableP + *fg_consoleP, 0);
            delay_tenths(1);
            setledstateP(kbd_tableP + *fg_consoleP, 7); /* two */
            delay_tenths(1);
            setledstateP(kbd_tableP + *fg_consoleP, 0);
            delay_tenths(1);
            setledstateP(kbd_tableP + *fg_consoleP, 7); /* three */
            delay_tenths(1);
            setledstateP(kbd_tableP + *fg_consoleP, 0);
            delay_tenths(1);
            break;

          case '-': leds = 8; /* back to lock-lights */

          default: break; /* nothing */
        }
        setledstateP(kbd_tableP + *fg_consoleP, leds);
    }

    return count;

}

/* =================================== select */

int kiss_select(struct inode *inode, struct file *filp, int mode,
                select_table *table)
{
    if (mode == SEL_EX)
        return 0;
    return 1; /* always readable and writable */
}

/* =================================== lseek */

int kiss_lseek(struct inode *inode, struct file *filp, off_t off,
               int whence)
{
    return -ESPIPE;
}

/* =================================== all the operations */

struct file_operations kiss_fops = {
    kiss_lseek,
    kiss_read,
    kiss_write,
    NULL,          /* kiss_readdir */
    kiss_select,          /* kiss_select */
    NULL,          /* kiss_ioctl */
    NULL,          /* kiss_mmap */
    kiss_open,
    kiss_close,
    NULL,          /* kiss_fsync */
    NULL,          /* kiss_fasync */
                   /* nothing more, fill with NULLs */
    NULL,
};


/* =================================== init and cleanup */

int init_module(void)
{
    int retval;
    kiss_dev.minor = MISC_DYNAMIC_MINOR;
    kiss_dev.name = "kiss";
    kiss_dev.fops = &kiss_fops;
    retval = misc_register(&kiss_dev);
    if (retval) return retval;
    return 0;
}

void cleanup_module(void)
{
    misc_deregister(&kiss_dev);
}

