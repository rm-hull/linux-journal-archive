/*
 * burp.c -- create /proc/root (block device),
 *                  /proc/insmod (kerneld requester)
 *              and /proc/jiffies (informational file)
 * tested under 2.0 on 486, alpha, sparc
 *
 * Copyright (C) 1997 Alessandro Rubini (rubini@linux.it)
 *
 * You can do whatever you want with this source, but there is
 * no warranty whatsoever, neither expressed nor implied.
 */

#ifndef __KERNEL__
#  define __KERNEL__
#endif
#ifndef MODULE
#  define MODULE
#endif

#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/errno.h>
#include <linux/kerneld.h>

/* 
 * the root device doesn't need anything but fill_ino
 */

void burp_root_fill_ino (struct inode *inode)
{
    /* the first superblock in the list of mounted disks is it */
    inode->i_rdev = super_blocks->s_dev;
}

struct proc_dir_entry burp_proc_root = {
    0,                  /* low_ino: the inode -- dynamic */
    4, "root",          /* len of name and name */
    S_IFBLK | 0600,     /* mode: block device, r/w by owner */
    1, 0, 0,            /* nlinks, owner (root), group (root) */
    0, &blkdev_inode_operations,  /* size (unused), inode ops */
    NULL,               /* get_info: unused */
    burp_root_fill_ino, /* fill_inode: tell your major/minor */
    /* nothing more */
};

/*
 * the insmod node needs a write, therefore the fops, therefore the iops
 */

int burp_insmod_write (struct inode *inode, struct file *filp,
                const char *buf, int count)
{
    char localbuf[64];
    int result;

    memset(localbuf, 0, 64); /* zero the string and copy the arg over it */
    if (count > 63) count = 63;
    memcpy_fromfs(localbuf, buf, count);

    if (localbuf[count-1] == '\n') /* trim the newline */
        localbuf[count-1] = '\0';

    if (localbuf[0] == '-')
        result = release_module(localbuf+1, 1 /* wait */);
    else
        result = request_module(localbuf);

    if (result == 0)
        return count; /* ok */
    if (result < 0)
        return result; /* kernel error */
    return -ENOENT; /* no such file or directory */
}

struct file_operations burp_insmod_fops = {
    NULL,          /* insmod_lseek */
    NULL,          /* insmod_read */
    burp_insmod_write,
};

struct inode_operations burp_insmod_iops = {
    &burp_insmod_fops,
};

struct proc_dir_entry burp_proc_insmod = {
    0,                  /* low_ino: the inode -- dynamic */
    6, "insmod",        /* len of name and name */
    S_IFREG | S_IWUGO,  /* mode: REGular, Write UserGroupOther */
    1, 0, 0,            /* nlinks, owner (root), group (root) */
    0, &burp_insmod_iops, /* size - unused; inode ops */
};

/*
 * The jiffies node uses the info pointer: no iops nor fops
 */

int burp_read_jiffies(char *buf, char **start, off_t off, int len, int unused)
{
    /* buf is a PAGE_SIZE buffer in kernel space */
    return (sprintf(buf, "%010li\n",jiffies));
}

struct proc_dir_entry burp_proc_jiffies = {
    0,                  /* low_ino: the inode -- dynamic */
    7, "jiffies",       /* len of name and name */
    S_IFREG | S_IRUGO,  /* mode: regular, read by anyone */
    1, 0, 0,            /* nlinks, owner (root), group (root) */
    11, NULL,           /* size is 11; inode ops unused */
    burp_read_jiffies,  /* use "info" instead */
};

/* finally, the real engines */

int init_module(void)
{
    proc_register_dynamic(&proc_root, &burp_proc_root);
    proc_register_dynamic(&proc_root, &burp_proc_insmod);
    proc_register_dynamic(&proc_root, &burp_proc_jiffies);
    register_symtab(NULL); /* hide symbols */
    return 0;
}

void cleanup_module(void)
{
    proc_unregister(&proc_root, burp_proc_root.low_ino);
    proc_unregister(&proc_root, burp_proc_insmod.low_ino);
    proc_unregister(&proc_root, burp_proc_jiffies.low_ino);
}
