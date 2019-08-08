/* bloom.c: Binary Loader for Outrageously Ostentatious Modules */

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

/*
 * the viewer can be modified at load time
 */
static char *viewer = "/usr/bin/X11/xv";
/*
 * this is the base name, calculated at first invocation
 */
static char *viewer_name;

static inline int do_load_bloom_binary(struct linux_binprm *bprm,
				       struct  pt_regs *regs)
{
    int retval=-ENOEXEC;    
    unsigned char *b = bprm->buf;

/*         Accept the following formats (/usr/lib/magic):
 * 
 *    PBMPLUS
 * 0       string    P1            PBM image text
 * 0       string    P2            PGM image text
 * 0       string    P3            PPM image text
 * 0       string    P4            PBM "rawbits" image data
 * 0       string    P5            PGM "rawbits" image data
 * 0       string    P6            PPM "rawbits" image data
 *    TIFF
 * 0       string    MM\x00\x2a    TIFF image data, big-endian
 * 0       string    II\x2a\x00    TIFF image data, little-endian
 *    GIF
 * 0       string    GIF8          GIF image data
 *    JPEG images
 * 0       beshort   0xffd8        JPEG image data
 * 
 */

    if (((b[0]=='P') && ((b[2]==' ') || (b[2]=='\n')) \
         && (b[1]>'0') && (b[1]<'7'))
        ||
        ((b[0]==b[1]) && (b[2]+b[3]==0x2a) && (b[2]*b[3]==0) \
         && ((b[0]=='M') || (b[0]=='I')))
        ||
        (*(u32 *)b == *(u32 *)"GIF8")
        ||
        ((b[0]==0xff) && (b[1]==0xd8))
        )
        retval=0;
    else
        return retval;

    if (!viewer_name) /* calculate the base name */
        viewer_name = strrchr(viewer,'/')+1;

    /* release the inode, it will be opened as argument */
    iput(bprm->inode);
    bprm->dont_iput=1;

    /* replace argv0 with the full pathname */
    remove_arg_zero(bprm);
    bprm->p = copy_strings(1, &bprm->filename, bprm->page, bprm->p, 2);
    bprm->argc++;

    /* and push the viewer's basename to the top of the command line */
    bprm->p = copy_strings(1, &viewer_name, bprm->page, bprm->p, 2);
    bprm->argc++;

    /* very unlikely it's more than 32 pages... */
        if (!bprm->p)
                return -E2BIG;

    /* ok, now open the interpreter, and reenter the loop */
    retval = open_namei(viewer, 0, 0, &bprm->inode, NULL);
    if (retval)
        return retval;
    bprm->dont_iput=0;
    retval=prepare_binprm(bprm);
    if(retval<0)
        return retval;

    return search_binary_handler(bprm,regs);
}

int load_bloom_binary(struct linux_binprm *bprm, struct  pt_regs *regs)
{
    int retval;
    MOD_INC_USE_COUNT;
    retval = do_load_bloom_binary(bprm,regs);
    MOD_DEC_USE_COUNT;
    return retval;
}

struct linux_binfmt bloom_format = {
    NULL, &mod_use_count_, /* next, count */
    load_bloom_binary, NULL, NULL /* bin, lib, core */
};

int init_module (void) {
    return register_binfmt(&bloom_format);
}

void cleanup_module(void) {
    unregister_binfmt(&bloom_format);
}
