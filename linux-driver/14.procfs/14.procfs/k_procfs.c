#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/proc_fs.h>

static struct proc_dir_entry *proc_dir;
static struct proc_dir_entry *proc_file;

static ssize_t k_read(struct file *File, char __user *user_buffer, size_t count, loff_t *offs)
{
	char buf[] = "procfs read function\n";

	return simple_read_from_buffer(user_buffer, count, offs, buf, strlen(buf));
}

static ssize_t k_write(struct file *File, const char __user *user_buffer, size_t count, loff_t *offs)
{
	char buf[255];
	int to_copy, not_copied, delta;

	memset(buf, 0, sizeof(buf));

	to_copy = min(count, sizeof(buf));

	not_copied = copy_from_user(buf, user_buffer, to_copy);
	pr_info("procfs: %s \n", buf);

	delta = to_copy - not_copied;

	return delta;
}

static struct proc_ops fops = {
	.proc_read = k_read,
	.proc_write = k_write,
};

static int __init k_module_init(void)
{
	proc_dir = proc_mkdir("k_proc", NULL);
	if(proc_dir == NULL) {
		pr_info("procfs_test - Error creating /proc/k_proc\n");
		return -ENOMEM;
	}

	proc_file = proc_create("k_file", 0666, proc_dir, &fops);
	if(proc_file == NULL) {
		printk("procfs_test - Error creating /proc/hello/k_file\n");
		proc_remove(proc_dir);
		return -ENOMEM;
	}

	return 0;
}

static void __exit k_module_exit(void)
{
	proc_remove(proc_file);
	proc_remove(proc_dir);
	pr_info("바이!!n");
}

module_init(k_module_init);
module_exit(k_module_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("K <k@k.com>");
MODULE_DESCRIPTION("k procfs module");
MODULE_VERSION("1.0.0");
