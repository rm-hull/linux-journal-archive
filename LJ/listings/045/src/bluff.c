/* bluff.c: Binary Loader for an Ultimately Fallacious Format */

#ifndef __KERNEL__
#  define __KERNEL__
#endif
#ifndef MODULE
#  define MODULE
#endif

#include <linux/stddef.h> /* NULL */
#include <linux/module.h>
#include <linux/binfmts.h>

struct linux_binfmt bluff_format = {
    NULL, &mod_use_count_, /* next, count */
    NULL, NULL, NULL       /* bin, lib, core */
};

int init_module (void) {
    return register_binfmt(&bluff_format);
}

void cleanup_module(void) {
    unregister_binfmt(&bluff_format);
}
