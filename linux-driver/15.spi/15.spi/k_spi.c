#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/version.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#include <linux/kernel.h>
#include <linux/delay.h>
#include <linux/spi/spi.h>

#define BUF_SIZE 1024

#define DRIVER_NAME "k_spi_driver"
#define DRIVER_CLASS "k_spi_class"

static dev_t k_dev;
static struct class *k_class;
static struct cdev k_device;

#define TOY_SPI_BUS_NUM 0
static struct spi_device *bmp280_dev;

struct spi_board_info spi_device_info = {
	.modalias = "bmp280",
	.max_speed_hz = 1000000,
	.bus_num = TOY_SPI_BUS_NUM,
	.chip_select = 0,
	.mode = 3,
};

static ssize_t k_driver_read(struct file *filp, char __user *buf, size_t count, loff_t *offset)
{
	pr_info("read\n");
	return count;
}

ssize_t k_driver_write(struct file *filp, const char __user *buf, size_t count, loff_t *offset)
{
	pr_info("write\n");
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
	u8 id;
	struct spi_master *master;

	if( alloc_chrdev_region(&k_dev, 0, 1, DRIVER_NAME) < 0) {
		pr_info("Device Nr. could not be allocated!\n");
		return -1;
	}

	if((k_class = class_create(THIS_MODULE, DRIVER_CLASS)) == NULL) {
		pr_info("Device class can not be created!\n");
		goto cerror;
	}

	if(device_create(k_class, NULL, k_dev, NULL, DRIVER_NAME) == NULL) {
		pr_info("Can not create device file!\n");
		goto device_error;
	}

	cdev_init(&k_device, &fops);

	if(cdev_add(&k_device, k_dev, 1) == -1) {
		pr_err("Registering of device to kernel failed!\n");
		goto reg_error;
	}

	master = spi_busnum_to_master(TOY_SPI_BUS_NUM);
	if (!master) {
		pr_err("Error! spi bus with Nr. %d\n", TOY_SPI_BUS_NUM);
		goto reg_error;
	}

	bmp280_dev = spi_new_device(master, &spi_device_info);
	if(!bmp280_dev) {
		pr_info("Could not spi create device!\n");
		goto reg_error;
	}

	bmp280_dev->bits_per_word = 8;

	if(spi_setup(bmp280_dev) != 0){
		pr_info("Could not change bus setup!\n");
		spi_unregister_device(bmp280_dev);
		goto spi_error;
	}

	id = spi_w8r8(bmp280_dev, 0xD0);
	pr_info("ID: 0x%x\n", id);

	return 0;

spi_error:
	spi_unregister_device(bmp280_dev);
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
	if(bmp280_dev)
		spi_unregister_device(bmp280_dev);
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
MODULE_DESCRIPTION("k spi");
MODULE_VERSION("1.0.0");