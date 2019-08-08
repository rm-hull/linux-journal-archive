/* fail.c: Format for Automatically Iinterpreting Lisp */

/* This code is released under the GPL license */
/* ------------------------------------------- */

#ifndef __KERNEL__
#  define __KERNEL__
#endif
#ifndef MODULE
#  define MODULE
#endif

#include <linux/sched.h>
#include <linux/module.h>
#include <linux/binfmts.h>

/* the viewer can be modified at load time */
static char *lisper = "/usr/bin/emacs";

/* this is "emacs -batch -l", "emacs" is derived from above */
#define NR_ARGS 3
static char *xtra_args[NR_ARGS]={NULL,"-batch","-l"};

static inline int do_load_fail_binary(struct linux_binprm *bprm,
				       struct  pt_regs *regs)
{
    int retval=-ENOEXEC;    
    unsigned char *b = bprm->buf;

/* A lisp file is identifiied by these magic sequences:
 *
 * 0    string  ;;                      Lisp/Scheme program text
 * 0    string  \012(                   byte-compiled Emacs-Lisp program data
 * 0    string  ;ELC\023\000\000\000    byte-compiled Emacs-Lisp program data
 */

    if (((b[0]==';') && (b[1]==';'))
        ||
        ((b[0]=='\012') && (b[1]=='('))
        ||
        ((b[0]==';') && (b[1]=='E') && (b[2]=='L') && (b[3]=='C') \
          && (b[4]=='\023') && (b[5]=='\0'))
        )
        retval=0;
    else
        return retval;

    if (!xtra_args[0]) /* calculate the base name */
        xtra_args[0] = strrchr(lisper,'/')+1;

    /* release the inode, it will be opened as argument */
    iput(bprm->inode);
    bprm->dont_iput=1;

    /* replace argv0 with the full pathname */
    remove_arg_zero(bprm);
    bprm->p = copy_strings(1, &bprm->filename, bprm->page, bprm->p, 2);
    bprm->argc++;

    /* add "emacs -batch" in front of it */
    bprm->p = copy_strings(NR_ARGS, xtra_args, bprm->page, bprm->p, 2);
    bprm->argc+=NR_ARGS;

    /* very unlikely it's more than 32 pages... */
        if (!bprm->p)
                return -E2BIG;

    /* ok, now open the interpreter, and reenter the loop */
    retval = open_namei(lisper, 0, 0, &bprm->inode, NULL);
    if (retval)
        return retval;
    bprm->dont_iput=0;
    retval=prepare_binprm(bprm);
    if(retval<0)
        return retval;

    return search_binary_handler(bprm,regs);
}

int load_fail_binary(struct linux_binprm *bprm, struct  pt_regs *regs)
{
    int retval;
    MOD_INC_USE_COUNT;
    retval = do_load_fail_binary(bprm,regs);
    MOD_DEC_USE_COUNT;
    return retval;
}

struct linux_binfmt fail_format = {
    NULL, &mod_use_count_, /* next, count */
    load_fail_binary, NULL, NULL /* bin, lib, core */
};

int init_module (void) {
    return register_binfmt(&fail_format);
}

void cleanup_module(void) {
    unregister_binfmt(&fail_format);
}
