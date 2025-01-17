#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/gpio.h>
#include <linux/kthread.h>
#include <linux/wait.h>
#include <linux/interrupt.h>
#include <linux/kernel.h>
#include <linux/delay.h>

#define BUF_SIZE	1024
#define WAIT_QUEUE_WAIT 0
#define WAIT_QUEUE_KEY 1
#define WAIT_QUEUE_NEXT 2
#define WAIT_QUEUE_EXIT 3
/* left leds */
#define TOY_GPIO_OUTPUT_LEFT_1 17
#define TOY_GPIO_OUTPUT_LEFT_2 27
#define TOY_GPIO_OUTPUT_LEFT_3 22
#define TOY_GPIO_OUTPUT_LEFT_4 5
/* right leds */
#define TOY_GPIO_OUTPUT_RIGHT_1 6
#define TOY_GPIO_OUTPUT_RIGHT_2 26
#define TOY_GPIO_OUTPUT_RIGHT_3 23
#define TOY_GPIO_OUTPUT_RIGHT_4 24
#define TOY_GPIO_INPUT 16

static char kernel_write_buffer[BUF_SIZE];
static dev_t toy_dev;
static struct class *toy_class;
static struct cdev toy_device;
static struct task_struct *wait_thread;
int wait_queue_flag = WAIT_QUEUE_WAIT;
unsigned int button_irq;

static int register_gpio_output(int gpio_num);

DECLARE_WAIT_QUEUE_HEAD(wait_queue);

#define DRIVER_NAME "toy_simple_io_driver"
#define DRIVER_CLASS "toy_simple_io_class"
#define ON  1
#define OFF 0

#define LED_LEFT_ON() 	\
do {	\
	gpio_set_value(TOY_GPIO_OUTPUT_LEFT_1, 1);	\
	gpio_set_value(TOY_GPIO_OUTPUT_LEFT_2, 1);	\
	gpio_set_value(TOY_GPIO_OUTPUT_LEFT_3, 1);	\
	gpio_set_value(TOY_GPIO_OUTPUT_LEFT_4, 1);	\
} while (0)

#define LED_LEFT_OFF() 	\
do {	\
	gpio_set_value(TOY_GPIO_OUTPUT_LEFT_1, 0);	\
	gpio_set_value(TOY_GPIO_OUTPUT_LEFT_2, 0);	\
	gpio_set_value(TOY_GPIO_OUTPUT_LEFT_3, 0);	\
	gpio_set_value(TOY_GPIO_OUTPUT_LEFT_4, 0);	\
} while (0)

#define LED_RIGHT_ON() 	\
do {	\
	gpio_set_value(TOY_GPIO_OUTPUT_RIGHT_1, 1);	\
	gpio_set_value(TOY_GPIO_OUTPUT_RIGHT_2, 1);	\
	gpio_set_value(TOY_GPIO_OUTPUT_RIGHT_3, 1);	\
	gpio_set_value(TOY_GPIO_OUTPUT_RIGHT_4, 1);	\
} while (0)

#define LED_RIGHT_OFF() 	\
do {	\
	gpio_set_value(TOY_GPIO_OUTPUT_RIGHT_1, 0);	\
	gpio_set_value(TOY_GPIO_OUTPUT_RIGHT_2, 0);	\
	gpio_set_value(TOY_GPIO_OUTPUT_RIGHT_3, 0);	\
	gpio_set_value(TOY_GPIO_OUTPUT_RIGHT_4, 0);	\
} while (0)

static ssize_t toy_driver_read(struct file *filp, char __user *buf, size_t count, loff_t *offset)
{

	return BUF_SIZE;
}

ssize_t toy_driver_write(struct file *filp, const char __user *buf, size_t count, loff_t *offset)
{
	if (copy_from_user(kernel_write_buffer, buf, count)) {
		pr_err("write: error\n");
	}

	switch (kernel_write_buffer[0]) {
		case 0:
			wait_queue_flag = WAIT_QUEUE_WAIT;
			pr_info("set WAIT_QUEUE_WAIT!\n");
			break;
		case 1:
			wait_queue_flag = WAIT_QUEUE_KEY;
			wake_up_interruptible(&wait_queue);
			pr_info("set WAIT_QUEUE_KEY!\n");
			break;
		case 2:
			wait_queue_flag = WAIT_QUEUE_NEXT;
			wake_up_interruptible(&wait_queue);
			pr_info("set WAIT_QUEUE_EXIT!\n");
			break;
		default:
			pr_info("Invalid Input!\n");
			break;
	}

	pr_info("write: done\n");

	return count;
}

static int toy_driver_open(struct inode *device_file, struct file *instance)
{
	pr_info("open\n");
	return 0;
}

static int toy_driver_close(struct inode *device_file, struct file *instance)
{
	pr_info("close\n");
	return 0;
}

static struct file_operations fops = {
	.owner = THIS_MODULE,
	.open = toy_driver_open,
	.release = toy_driver_close,
	.read = toy_driver_read,
	.write = toy_driver_write
};

int toy_simple_io_example_1(void)
{
	int ledvalue = OFF;

	pr_info("\n 토이 simple io 실험 1\n");;
	for (;;) {
		if (ledvalue == ON) {
			ledvalue = OFF;
			pr_info("LEFT LED OFF\n");
			LED_LEFT_OFF();
		} else {
			ledvalue = ON;
			pr_info("LEFT LED ON\n");
			LED_LEFT_ON();
		}
		wait_queue_flag = WAIT_QUEUE_WAIT;
		wait_event_interruptible(wait_queue, wait_queue_flag != WAIT_QUEUE_WAIT);
		if (wait_queue_flag == WAIT_QUEUE_EXIT || wait_queue_flag == WAIT_QUEUE_NEXT) {
			break;
		}
	}

	return wait_queue_flag;
}

int toy_simple_io_example_2(void)
{

	pr_info("\n 토이 simple io 실험 2\n");;
	for (;;) {
		wait_queue_flag = WAIT_QUEUE_WAIT;
		wait_event_interruptible(wait_queue, wait_queue_flag != WAIT_QUEUE_WAIT);
		if (wait_queue_flag == WAIT_QUEUE_EXIT || wait_queue_flag == WAIT_QUEUE_NEXT) {
			break;
		}
	}
	return wait_queue_flag;
}

int toy_simple_io_example_3(void)
{

	pr_info("\n 토이 simple io 실험 3\n");;
	wait_queue_flag = WAIT_QUEUE_WAIT;
	for (;;) {

		wait_queue_flag = WAIT_QUEUE_WAIT;
		wait_event_interruptible(wait_queue, wait_queue_flag != WAIT_QUEUE_WAIT);
		if (wait_queue_flag == WAIT_QUEUE_EXIT || wait_queue_flag == WAIT_QUEUE_NEXT) {
			break;
		}
	}
	return wait_queue_flag;
}

int toy_simple_io_example_4(void)
{

	pr_info("\n 토이 simple io 실험 4\n");;
	wait_queue_flag = WAIT_QUEUE_WAIT;
	for (;;) {
		usleep_range(500000, 500001);
		if (wait_queue_flag == WAIT_QUEUE_EXIT || wait_queue_flag == WAIT_QUEUE_NEXT) {
			break;
		}
	}
	return wait_queue_flag;
}

int toy_simple_io_example_5(void)
{
	int sw_tact_key = 0;

	pr_info("\n 토이 simple io 실험 5\n");;
	wait_queue_flag = WAIT_QUEUE_WAIT;
	for (;;) {
		sw_tact_key = gpio_get_value(TOY_GPIO_INPUT);
		if (sw_tact_key == 1) {
			LED_LEFT_ON();
			LED_RIGHT_OFF();
		} else {
			LED_RIGHT_ON();
			LED_LEFT_OFF();
		}
		usleep_range(1000, 1001);
		if (wait_queue_flag == WAIT_QUEUE_EXIT || wait_queue_flag == WAIT_QUEUE_NEXT) {
			break;
		}
	}
	return wait_queue_flag;
}

int toy_simple_io_example_6(void)
{
	pr_info("\n 토이 simple io 실험 6\n");;
	wait_queue_flag = WAIT_QUEUE_WAIT;
	for (;;) {
		if (wait_queue_flag == WAIT_QUEUE_EXIT || wait_queue_flag == WAIT_QUEUE_NEXT) {
			break;
		}
	    usleep_range(10000, 10001);
	}
	return wait_queue_flag;
}

static int simple_io_kthread(void *unused)
{
	while (1) {
		if (toy_simple_io_example_1() == WAIT_QUEUE_EXIT)
			break;
		if (toy_simple_io_example_2() == WAIT_QUEUE_EXIT)
			break;
		if (toy_simple_io_example_3() == WAIT_QUEUE_EXIT)
			break;
		if (toy_simple_io_example_4() == WAIT_QUEUE_EXIT)
			break;
		if (toy_simple_io_example_5() == WAIT_QUEUE_EXIT)
			break;
		if (toy_simple_io_example_6() == WAIT_QUEUE_EXIT)
			break;
	}
	do_exit(0);
	return 0;
}

static int register_gpio_output(int gpio_num)
{
	char name[80];

	snprintf(name, sizeof(name), "toy-gpio-%d", gpio_num);

	if (gpio_request(gpio_num, name)) {
		pr_info("Can not allocate GPIO 17\n");
		return -1;
	}

	if(gpio_direction_output(gpio_num, 0)) {
		pr_info("Can not set GPIO 16 to output!\n");
		return -1;
	}

	return 0;
}

static int __init toy_module_init(void)
{

	/* 여기서 노드를 할당 받는다. */
	if (alloc_chrdev_region(&toy_dev, 0, 1, DRIVER_NAME) < 0) {
		pr_info("Device Nr. could not be allocated!\n");
		return -1;
	}

	pr_info("할당 받은 Major = %d Minor = %d \n", MAJOR(toy_dev), MINOR(toy_dev));

	/* device class 생성 */
	if ((toy_class = class_create(THIS_MODULE, DRIVER_CLASS)) == NULL) {
		pr_info("Device class can not be created!\n");
		goto cerror;
	}

	/* device file 생성 */
	if (device_create(toy_class, NULL, toy_dev, NULL, DRIVER_NAME) == NULL) {
		pr_info("Can not create device file!\n");
		goto device_error;
	}

	/* character device file 초기화 */
	cdev_init(&toy_device, &fops);

	/* 커널에 등록 */
	if (cdev_add(&toy_device, toy_dev, 1) == -1) {
		pr_info("Registering of device to kernel failed!\n");
		goto reg_error;
	}

	register_gpio_output(TOY_GPIO_OUTPUT_LEFT_1);
	register_gpio_output(TOY_GPIO_OUTPUT_LEFT_2);
	register_gpio_output(TOY_GPIO_OUTPUT_LEFT_3);
	register_gpio_output(TOY_GPIO_OUTPUT_LEFT_4);
	register_gpio_output(TOY_GPIO_OUTPUT_RIGHT_1);
	register_gpio_output(TOY_GPIO_OUTPUT_RIGHT_2);
	register_gpio_output(TOY_GPIO_OUTPUT_RIGHT_3);
	register_gpio_output(TOY_GPIO_OUTPUT_RIGHT_4);

	if(gpio_request(TOY_GPIO_INPUT, "toy-gpio-16")) {
		pr_info("Can not allocate GPIO 16\n");
		goto gpio_17_error;
	}

	if(gpio_direction_input(TOY_GPIO_INPUT)) {
		pr_info("Can not set GPIO 16 to input!\n");
		goto gpio_16_error;
	}

	wait_thread = kthread_create(simple_io_kthread, NULL, "simple io thread");
	if (wait_thread) {
		pr_info("Thread created successfully\n");
		wake_up_process(wait_thread);
	} else
		pr_info("Thread creation failed\n");


	return 0;

gpio_16_error:
	gpio_free(16);
gpio_17_error:
	gpio_free(17);
reg_error:
	device_destroy(toy_class, toy_dev);
device_error:
	class_destroy(toy_class);
cerror:
	unregister_chrdev_region(toy_dev, 1);
	return -1;
}

static void __exit toy_module_exit(void)
{
	wait_queue_flag = WAIT_QUEUE_EXIT;
	wake_up_interruptible(&wait_queue);
	gpio_set_value(16, 0);
	gpio_free(16);
	gpio_free(TOY_GPIO_OUTPUT_LEFT_1);
	gpio_free(TOY_GPIO_OUTPUT_LEFT_2);
	gpio_free(TOY_GPIO_OUTPUT_LEFT_3);
	gpio_free(TOY_GPIO_OUTPUT_LEFT_4);
	gpio_free(TOY_GPIO_OUTPUT_RIGHT_1);
	gpio_free(TOY_GPIO_OUTPUT_RIGHT_2);
	gpio_free(TOY_GPIO_OUTPUT_RIGHT_3);
	gpio_free(TOY_GPIO_OUTPUT_RIGHT_4);
	cdev_del(&toy_device);
	device_destroy(toy_class, toy_dev);
	class_destroy(toy_class);
	unregister_chrdev_region(toy_dev, 1);
	pr_info("exit\n");
}

module_init(toy_module_init);
module_exit(toy_module_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("TOY <toy@toy.com>");
MODULE_DESCRIPTION("toy simple io");
MODULE_VERSION("1.0.0");
