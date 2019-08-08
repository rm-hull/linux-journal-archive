/* Filename: kprobes.c */

#include <linux/module.h>
#include <linux/init.h>
#include <linux/kprobes.h>

static struct kprobe kpr;
extern int hello_to_debug(void);

static void __exit exit_probe(void)
{
       printk("\nModule exiting \n");
       unregister_kprobe(&kpr);
}

static int before_hook(struct kprobe *kpr, 
		       struct pt_regs *p)
{
       printk("\nBefore hook");
       printk("\nThis is the Kprobe pre \n"
              "handler for instruction at \n"
	      "%p\n", kpr->addr);
       printk("The registers are:\n");
       printk("eax=%lx, ebx=%lx, ecx=%lx,\n "
              "edx=%lx\n", p->eax,  p->ebx,  
	      p->ecx,  p->edx);
       printk("eflags=%lx, esp=%lx\n",
               p->eflags,  p->esp);
       return 0;
}

static int after_hook(struct kprobe *kpr, 
		      struct pt_regs *p,
                      unsigned long flags)
{
       printk("\nAfter hook");
       printk("\nThis is the Kprobe post \n"
              "handler for instruction at"
	      " %p\n", kpr->addr);
       printk("The registers are:\n");
       printk("eax=%lx, ebx=%lx, ecx=%lx, \n"
              "edx=%lx\n", p->eax,  p->ebx,  
	      p->ecx,  p->edx);
       printk("eflags=%lx, esp=%lx\n",
               p->eflags,  p->esp);
       return 0;
}

static int __init init_probe(void)
{
       printk("\nInserting the kprobes \n");
       /* Registering a kprobe */
       kpr.pre_handler = 
	   (kprobe_pre_handler_t)before_hook;
       kpr.post_handler = 
	   (kprobe_post_handler_t)after_hook;
       kpr.addr = 
	  (kprobe_opcode_t *)(&hello_to_debug);
       printk("\nAddress where the kprobe is \n"
              "going to be inserted - %p \n", 
	      kpr.addr);
       register_kprobe(&kpr);
       return 0;
}

module_init(init_probe);
module_exit(exit_probe);

MODULE_AUTHOR ("Krishnakumar. R, 
               <rkrishnakumar@gmail.com>");
MODULE_DESCRIPTION ("Kprobes test module");
MODULE_LICENSE("GPL");

