#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/gpio.h>
#include <linux/kthread.h>
#include <linux/wait.h>
#include <linux/interrupt.h>
#include <linux/timer.h>

#define K_GPIO_OUTPUT 17
#define K_GPIO_INPUT 16
#define BUF_SIZE	1024

static dev_t k_dev;
static struct class *k_class;
static struct cdev k_device;
unsigned int button_irq;

#define DRIVER_NAME "k_wq_list_driver"
#define DRIVER_CLASS "k_wq_list_class"

static struct workqueue_struct *k_workqueue;
static void workqueue_fn(struct work_struct *work);

unsigned long pre_jiffie = 0;
unsigned int led = 0;

static DEFINE_RWLOCK(k_rwlock);

struct k_list {
	struct list_head list;
	int data;
};

LIST_HEAD(k_list_head);
static DECLARE_WORK(work, workqueue_fn);
volatile int k_value = 0;

static void workqueue_fn(struct work_struct *work)
{
	struct k_list *new_node = NULL;

	if (in_interrupt()) {
		pr_info("workqueue_fn: interrupt conext 입니다.!!!\n");
	} else {
		pr_info("workqueue_fn: thread conext 입니다.!!!\n");
	}
	new_node = kmalloc(sizeof(struct k_list), GFP_KERNEL);
	new_node->data = k_value++;
	INIT_LIST_HEAD(&new_node->list);
	write_lock(&k_rwlock);
	list_add_tail(&new_node->list, &k_list_head);
	write_unlock(&k_rwlock);
}

static irq_handler_t k_top_half_gpio_irq_signal_handler(unsigned int irq, void *dev_id)
{
	unsigned long diff = jiffies - pre_jiffie;

	if (in_interrupt()) {
		pr_info("k_top_half_gpio_irq_signal_handler: interrupt conext 입니다.!!!\n");
	} else {
		pr_info("k_top_half_gpio_irq_signal_handler: thread conext 입니다.!!!\n");
	}

	/* 200 msec 이내에 호출한 인터럽트는 무시 */
	if (diff < msecs_to_jiffies(200)) {
		return (irq_handler_t) IRQ_HANDLED;
	}

	pre_jiffie = jiffies;

	queue_work(k_workqueue, &work);
	/*
	 * 만약 리턴 값이 IRQ_HANDLED 이면, k_bottom_half_gpio_irq_signal_handler 호출 안함.
     * IRQ_WAKE_THREAD이면 bottom half thread 호출함.
	*/
	return (irq_handler_t)IRQ_WAKE_THREAD;
}

static irq_handler_t k_bottom_half_gpio_irq_signal_handler(unsigned int irq, void *dev_id)
{
	if (in_interrupt()) {
		pr_info("k_top_half_gpio_irq_signal_handler: interrupt conext 입니다.!!!\n");
	} else {
		pr_info("k_top_half_gpio_irq_signal_handler: thread conext 입니다.!!!\n");
	}

	pr_info("k_gpio_irq_signal_handler: interrupt triggered!!!\n");

	led = (0x01 ^ led);
	gpio_set_value(K_GPIO_OUTPUT, led);
	pr_info("Interrupt(Threaded Handler) : K_GPIO_OUTPUT : %d ",gpio_get_value(K_GPIO_OUTPUT));

	return (irq_handler_t) IRQ_HANDLED;
}

static ssize_t k_driver_read(struct file *filp, char __user *buf, size_t count, loff_t *offset)
{
	struct k_list *temp;
	int list_count = 0;

	read_lock(&k_rwlock);
	list_for_each_entry(temp, &k_list_head, list) {
		pr_info("list_count : %d, data : %d\n", list_count++, temp->data);
	}
	read_unlock(&k_rwlock);

	pr_info("Node count: %d\n", list_count);
	return BUF_SIZE;
}

ssize_t k_driver_write(struct file *filp, const char __user *buf, size_t count, loff_t *offset)
{
	pr_info("write: done\n");

	return count;
}

static int k_driver_open(struct inode *device_file, struct file *instance)
{
	pr_info("open\n");
	return 0;
}

static int k_driver_close(struct inode *device_file, struct file *instance)
{
	pr_info("close\n");
	return 0;
}

static struct file_operations fops = {
	.owner = THIS_MODULE,
	.open = k_driver_open,
	.release = k_driver_close,
	.read = k_driver_read,
	.write = k_driver_write
};

static int __init k_module_init(void)
{
	if (alloc_chrdev_region(&k_dev, 0, 1, DRIVER_NAME) < 0) {
		pr_info("Device Nr. could not be allocated!\n");
		return -1;
	}

	if ((k_class = class_create(THIS_MODULE, DRIVER_CLASS)) == NULL) {
		pr_info("Device class can not be created!\n");
		goto cerror;
	}

	if (device_create(k_class, NULL, k_dev, NULL, DRIVER_NAME) == NULL) {
		pr_info("Can not create device file!\n");
		goto device_error;
	}

	cdev_init(&k_device, &fops);

	if (cdev_add(&k_device, k_dev, 1) == -1) {
		pr_info("Registering of device to kernel failed!\n");
		goto reg_error;
	}

	if (gpio_request(K_GPIO_OUTPUT, "k-gpio-17")) {
		pr_info("Can not allocate GPIO 17\n");
		goto reg_error;
	}

	if(gpio_direction_output(K_GPIO_OUTPUT, 0)) {
		pr_info("Can not set GPIO 16 to output!\n");
		goto gpio_17_error;
	}

	if(gpio_request(K_GPIO_INPUT, "k-gpio-16")) {
		pr_info("Can not allocate GPIO 16\n");
		goto gpio_17_error;
	}

	if(gpio_direction_input(K_GPIO_INPUT)) {
		pr_info("Can not set GPIO 16 to input!\n");
		goto gpio_16_error;
	}

	button_irq = gpio_to_irq(16);

	if (request_threaded_irq(button_irq, (void *)k_top_half_gpio_irq_signal_handler, 
			(void *)k_bottom_half_gpio_irq_signal_handler, IRQF_TRIGGER_RISING, "k_gpio_irq_signal", NULL) != 0) {
		pr_err("Error!\n Can not request interrupt nr: %d\n", button_irq);
		gpio_free(17);
		return -1;
	}

	k_workqueue = create_workqueue("k_wq");

	return 0;

gpio_16_error:
	gpio_free(16);
gpio_17_error:
	gpio_free(17);
reg_error:
	device_destroy(k_class, k_dev);
device_error:
	class_destroy(k_class);
cerror:
	unregister_chrdev_region(k_dev, 1);
	return -1;
}

static void __exit k_module_exit(void)
{
	struct k_list *lnode, *temp;
	write_lock(&k_rwlock);
	list_for_each_entry_safe(lnode, temp, &k_list_head, list) {
		list_del(&lnode->list);
		kfree(lnode);
	}
	write_unlock(&k_rwlock);

	destroy_workqueue(k_workqueue);
	free_irq(button_irq, NULL);
	gpio_set_value(16, 0);
	gpio_free(16);
	gpio_free(17);
	cdev_del(&k_device);
	device_destroy(k_class, k_dev);
	class_destroy(k_class);
	unregister_chrdev_region(k_dev, 1);
	pr_info("exit\n");
}

module_init(k_module_init);
module_exit(k_module_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("K <k@k.com>");
MODULE_DESCRIPTION("k work queue");
MODULE_VERSION("1.0.0");
