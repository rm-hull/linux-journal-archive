/* Filename: call.c */

#include <linux/module.h>
#include <linux/init.h>

extern int hello_to_debug(void);

static void __exit exit_to_debug(void)
{
        printk("\nModule exiting \n");
}

static int __init init_to_debug(void)
{
        printk("\nCalling the function \n");
        hello_to_debug();
        return 0;
}

module_init(init_to_debug);
module_exit(exit_to_debug);

MODULE_AUTHOR ("Krishnakumar. R, 
               <rkrishnakumar@gmail.com>");
MODULE_DESCRIPTION ("Kprobes test module");
MODULE_LICENSE("GPL");
