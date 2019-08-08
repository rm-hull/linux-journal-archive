/* Filename: first.c */

#include <linux/module.h>
#include <linux/init.h>

int hello_to_debug(void)
{
        printk("\nFrom the function - %s\n", 
			      __FUNCTION__);
        return 0;
}

static void exit_to_debug(void)
{
        printk("\nModule exiting \n");
}

static int init_to_debug(void)
{
        printk("\nKeeping the function to debug"
               "\nat the kernel address %p\n", 
	       hello_to_debug);
	return 0;
}

EXPORT_SYMBOL(hello_to_debug);
module_init(init_to_debug);
module_exit(exit_to_debug);

MODULE_AUTHOR ("Krishnakumar. R, 
	       <rkrishnakumar@gmail.com>");
MODULE_DESCRIPTION ("Kprobes test module");
MODULE_LICENSE("GPL");
