#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/syscalls.h>
#include <linux/file.h>
#include <linux/fs.h>
#include <linux/fcntl.h>
#include <asm/uaccess.h>

static void write_file(char *filename, char *data)
{
	struct file *file;
	loff_t pos = 0;
	int fd;

	mm_segment_t old_fs = get_fs();
	set_fs(KERNEL_DS);

	fd = sys_open(filename, O_WRONLY | O_CREAT, 0644);
	if (fd >= 0) {
		file = fget(fd);
		if (file) {
			vfs_write(file, data, strlen(data), &pos);
			fput(file);
		}
		sys_close(fd);
	}
	set_fs(old_fs);
}

static int __init init(void)
{
	write_file("/tmp/test", "This is a line.\n");
	return 0;
}

static void __exit exit(void)
{
}

MODULE_LICENSE("GPL");

module_init(init);
module_exit(exit);
