#include <linux/init.h>    /* For module_init and module_clea */
#include <linux/module.h>  /* Needed by all modules */
#include <linux/kernel.h>  /* Needed for KERN_ALERT */

/* For socket etc */
#include <linux/net.h>
#include <net/sock.h>
#include <linux/tcp.h>
#include <linux/in.h>
#include <asm/uaccess.h>
#include <linux/file.h>
#include <linux/socket.h>
#include <linux/smp_lock.h>
#include <linux/slab.h> /* for kmalloc */

#define FTP_STRING "FTP_CLIENT: "

#define PORT 21     /* FTP port */
#define EPH_PORT 2000 /* Ephemeral port for client */
#define SNDBUF 4096 
#define RCVBUF 4096

#include "ftp.h"

/* A simple function to convert the address from familiar 
 * 4 part form to a 32-bit number */
u32 create_address(u8 *ip)
{
    u32 addr = 0;
    int i;
    for(i = 0;i < 4;++i) {
	    addr += ip[i];
	    if(i == 3) 
		    break;
	    addr <<= 8;
    }
    return addr;
}

int send_sync_buf (struct socket *sock, const char *buf, 
                   const size_t length, unsigned long flags)
{
    struct msghdr msg;
    struct iovec iov;
    int len, written = 0, left = length;
    mm_segment_t oldmm;

    msg.msg_name     = 0;
    msg.msg_namelen  = 0;
    msg.msg_iov      = &iov;
    msg.msg_iovlen   = 1;
    msg.msg_control  = NULL;
    msg.msg_controllen = 0;
    msg.msg_flags    = flags;

    oldmm = get_fs(); set_fs(KERNEL_DS);

repeat_send:
    msg.msg_iov->iov_len = left;
    msg.msg_iov->iov_base = (char *) buf + written;

    len = sock_sendmsg(sock, &msg, left);
    if ((len == -ERESTARTSYS) || (!(flags & MSG_DONTWAIT) && 
         (len == -EAGAIN)))
        goto repeat_send;
    if (len > 0) {
        written += len;
        left -= len;
        if (left)
            goto repeat_send;
    }
    set_fs(oldmm);
    return written ? written : len;
}

void send_reply(struct socket *sock, char *str)
{   
    send_sync_buf(sock, str, strlen(str), MSG_DONTWAIT);
}

int read_response(struct socket *sock, char *str)
{       mm_segment_t oldmm;
        struct msghdr msg;
        struct iovec iov;
        int len;
        int max_size = SNDBUF;

        msg.msg_name     = 0;
        msg.msg_namelen  = 0;
        msg.msg_iov      = &iov;
        msg.msg_iovlen   = 1;
        msg.msg_control  = NULL;
        msg.msg_controllen = 0;
        msg.msg_flags    = 0;
        
        msg.msg_iov->iov_base = str;
        msg.msg_iov->iov_len  = max_size;
        
        oldmm = get_fs(); set_fs(KERNEL_DS);

read_again:
        len = sock_recvmsg(sock, &msg, max_size, 0); /*MSG_DONTWAIT); */
        if (len == -EAGAIN || len == -ERESTARTSYS) {
            goto read_again;
        }
        set_fs(oldmm);
        return len;
}

int write_to_file(char *fname, struct socket *sock, char *response)
{
    struct file *f = NULL;
    int flags, total_written;
    mm_segment_t oldmm;
    int len;

    flags = O_CREAT | O_WRONLY | O_TRUNC;
    if(!fname)
        return -1;
    f = filp_open(fname, flags, 0600);
    if (!f || !f->f_op || !f->f_op->write) {
        printk(KERN_ERR "WARNING: File (write) object is a null pointer!!!\n");
        return -1;
    }
    f->f_pos = 0;

    oldmm = get_fs(); set_fs(KERNEL_DS);
    total_written = 0;
    while((len = read_response(sock, response)) > 0) {
        if(signal_pending(current)) {
            printk(KERN_INFO FTP_STRING "Got a signal\n");
            total_written = -ERESTARTSYS;
            break;
        }
        len = f->f_op->write(f, response, len, &f->f_pos);
        total_written += len;
    }
    set_fs(oldmm);
    if (f->f_op && f->f_op->flush) {
        lock_kernel();
        f->f_op->flush(f);
        unlock_kernel();
    }
    fput(f);

    printk(KERN_INFO FTP_STRING "Wrote %d bytes\n", total_written);
    return total_written;
}

extern void *sys_call_table[];
asmlinkage long sys_ni_syscall(void)
{
    return -ENOSYS;
}


asmlinkage int my_sys_call(struct params __user *pm)
{
    struct sockaddr_in saddr, daddr;
    struct socket *control= NULL;
    struct socket *data = NULL;
    struct socket *new_sock = NULL;

    int r = -1;
    char *response = kmalloc(SNDBUF, GFP_KERNEL);
    char *reply = kmalloc(RCVBUF, GFP_KERNEL);
    
    char *a, *p;
    char address[128];
    int len = 0, total_written = 0;
    char temp[64];
 
    struct params pmk;

    if(unlikely(!access_ok(VERIFY_READ, pm, sizeof(pm))))
        return -EFAULT;
    if(copy_from_user(&pmk, pm, sizeof(struct params))) 
        return -EFAULT;
    if(current->uid != 0)
        return r;

    r = sock_create(PF_INET, SOCK_STREAM, IPPROTO_TCP, &control);
    if (r < 0) {
        printk(KERN_ERR FTP_STRING "error %d creating socket.\n", r);
        goto err;
    }
   
    memset(&saddr,0, sizeof(saddr));
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(PORT);
    saddr.sin_addr.s_addr = htonl(create_address(pmk.destip)); 

    r = control->ops->connect(control, (struct sockaddr *) &saddr, 
                          sizeof(saddr), O_RDWR);
    if (r && (r != -EINPROGRESS)) {
        printk(KERN_ERR FTP_STRING "connect error: %d\n", r);
        goto err;
    }
    read_response(control, response);
    sprintf(temp, "USER %s\r\n", pmk.user);
    send_reply(control, temp);
    read_response(control, response);
    sprintf(temp, "PASS %s\r\n", pmk.pass);
    send_reply(control, temp);
    read_response(control, response);

    r = sock_create(PF_INET, SOCK_STREAM, IPPROTO_TCP, &data);
    if (r < 0) {
        printk(KERN_ERR FTP_STRING "error %d creating data socket.\n", r);
        goto err1;
    }
    memset(&daddr,0, sizeof(daddr));
    daddr.sin_family = AF_INET;
    daddr.sin_port = htons(EPH_PORT);
    daddr.sin_addr.s_addr= htonl(create_address(pmk.srcip));
    r = data->ops->bind(data, (struct sockaddr *)&daddr, sizeof (daddr));
    if(r < 0) {
        printk(KERN_ERR FTP_STRING "error %d binding data socket.\n", r);
        goto err2;
    }
    
    r = data->ops->listen(data, 1);
    if(r < 0) {
        printk(KERN_ERR FTP_STRING "error %d listening in data socket.\n", r);
        goto err2;
    }
   
    a = (char *)&daddr.sin_addr;
    p = (char *)&daddr.sin_port;
#define UC(b)   (((int)b)&0xff)
    sprintf(reply, "PORT %d,%d,%d,%d,%d,%d\r\n",
            UC(a[0]), UC(a[1]), UC(a[2]), UC(a[3]),
            UC(p[0]), UC(p[1]));
    
    printk(KERN_INFO FTP_STRING "Sending PORT command: %s\n", reply);
    send_reply(control, reply);
    read_response(control, response);
   
    strcpy(reply, "RETR ");
    strcat(reply, pmk.src);
    strcat(reply, "\r\n");

    send_reply(control, reply);
    read_response(control, response);
    
    new_sock = sock_alloc();
    if (!new_sock)
        goto err2;
    new_sock->type = data->type;
    new_sock->ops = data->ops;

    r = data->ops->accept(data, new_sock, 0);
    if (r < 0) {
        printk("Error accepting connection on data socket\n");
        goto err3;
    }

    new_sock->ops->getname(new_sock, (struct sockaddr *)address, &len, 2);
    a = (char *)&(((struct sockaddr_in *)address)->sin_addr.s_addr);
    printk("Connection from %d.%d.%d.%d\n", UC(a[0]), UC(a[1]), UC(a[2]), UC(a[3]));
 
    if((total_written = write_to_file(pmk.dst, new_sock, response)) < 0)
        goto err3;
    sock_release(new_sock);
    sock_release(data);
    sock_release(control);
    return total_written;

err3:
    sock_release(new_sock);
err2:
    sock_release(data);
err1:
    sock_release(control);
err:
    return -1;
}

static int ftp_init(void)
{

    printk(KERN_INFO FTP_STRING "Starting ftp client module\n");
    sys_call_table[223] = my_sys_call;
    return 0;
}

static void ftp_exit(void)
{
    printk(KERN_INFO FTP_STRING "Cleaning up ftp client module, bye !\n");
    sys_call_table[223] = sys_ni_syscall;
} 

module_init(ftp_init);
module_exit(ftp_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Pradeep Padala");
