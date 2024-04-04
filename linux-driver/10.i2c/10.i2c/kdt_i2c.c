#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/version.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#include <linux/i2c.h>
#include <linux/kernel.h>
#include <linux/delay.h>

#define BUF_SIZE 1024

#define DRIVER_NAME "kdt_i2c_driver"
#define DRIVER_CLASS "kdt_i2c_class"

static char kernel_write_buffer[BUF_SIZE];
static dev_t kdt_dev;
static struct class *kdt_class;
static struct cdev kdt_device;

static struct i2c_adapter *bmp_i2c_adapter = NULL;
static struct i2c_client *bmp280_i2c_client = NULL;

#define I2C_BUS_AVAILABLE 1
#define SLAVE_DEVICE_NAME "KDT_BMP280"
#define BMP280_SLAVE_ADDRESS 0x76

static const struct i2c_device_id bmp_id[] = {
	{SLAVE_DEVICE_NAME, 0},
	{}};

static struct i2c_driver bmp_driver = {
	.driver = {
		.name = SLAVE_DEVICE_NAME,
		.owner = THIS_MODULE}
	};

static struct i2c_board_info bmp_i2c_board_info = {
	I2C_BOARD_INFO(SLAVE_DEVICE_NAME, BMP280_SLAVE_ADDRESS)};


s32 dig_T1, dig_T2, dig_T3;

s32 read_temperature(void)
{
	int var1, var2;
	s32 raw_temp;
	s32 d1, d2, d3;

	d1 = i2c_smbus_read_byte_data(bmp280_i2c_client, 0xFA);
	d2 = i2c_smbus_read_byte_data(bmp280_i2c_client, 0xFB);
	d3 = i2c_smbus_read_byte_data(bmp280_i2c_client, 0xFC);
	raw_temp = ((d1 << 16) | (d2 << 8) | d3) >> 4;
	pr_info("raw_temp: %d\n", raw_temp);

	var1 = ((((raw_temp >> 3) - (dig_T1 << 1))) * (dig_T2)) >> 11;

	var2 = (((((raw_temp >> 4) - (dig_T1)) * ((raw_temp >> 4) - (dig_T1))) >> 12) * (dig_T3)) >> 14;
	return ((var1 + var2) * 5 + 128) >> 8;
}

static ssize_t kdt_driver_read(struct file *filp, char __user *buf, size_t count, loff_t *offset)
{
	char out_string[20];
	int temperature;

	temperature = read_temperature();
	snprintf(out_string, sizeof(out_string), "%d.%d\n", temperature / 100, temperature % 100);

	pr_info("%s\n", out_string);
	if (copy_to_user(buf, out_string, count)) {
		pr_err("read: error!\n");
	}

	return count;
}

ssize_t kdt_driver_write(struct file *filp, const char __user *buf, size_t count, loff_t *offset)
{
	if (copy_from_user(kernel_write_buffer, buf, count)) {
		pr_err("write: error\n");
	}
	pr_info("write: done\n");

	return count;
}

static int kdt_driver_open(struct inode *device_file, struct file *instance)
{
	pr_info("open\n");
	return 0;
}

static int kdt_driver_close(struct inode *device_file, struct file *instance)
{
	pr_info("close\n");
	return 0;
}

static struct file_operations fops = {
	.owner = THIS_MODULE,
	.open = kdt_driver_open,
	.release = kdt_driver_close,
	.read = kdt_driver_read,
	.write = kdt_driver_write
};

static int __init kdt_module_init(void)
{
	u8 id;

	/* 여기서 노드를 할당 받는다. */
	if( alloc_chrdev_region(&kdt_dev, 0, 1, DRIVER_NAME) < 0) {
		pr_info("Device Nr. could not be allocated!\n");
		return -1;
	}

	pr_info("할당 받은 Major = %d Minor = %d \n", MAJOR(kdt_dev), MINOR(kdt_dev));

	/* device class 생성 */
	if((kdt_class = class_create(THIS_MODULE, DRIVER_CLASS)) == NULL) {
		pr_info("Device class can not be created!\n");
		goto cerror;
	}

	/* device file 생성 */
	if(device_create(kdt_class, NULL, kdt_dev, NULL, DRIVER_NAME) == NULL) {
		pr_info("Can not create device file!\n");
		goto device_error;
	}

	/* character device file 초기화 */
	cdev_init(&kdt_device, &fops);

	/* 커널에 등록 */
	if(cdev_add(&kdt_device, kdt_dev, 1) == -1) {
		pr_info("Registering of device to kernel failed!\n");
		goto reg_error;
	}

	bmp_i2c_adapter = i2c_get_adapter(I2C_BUS_AVAILABLE);

	if (bmp_i2c_adapter != NULL) {
		bmp280_i2c_client = i2c_new_client_device(bmp_i2c_adapter, &bmp_i2c_board_info);
		if (bmp280_i2c_client != NULL) {
			if (i2c_add_driver(&bmp_driver)) {
				pr_info("Can't add driver...\n");
				goto reg_error;
			}
		}
		i2c_put_adapter(bmp_i2c_adapter);
	}
	id = i2c_smbus_read_byte_data(bmp280_i2c_client, 0xD0);
	pr_info("ID: 0x%x\n", id);

	dig_T1 = i2c_smbus_read_word_data(bmp280_i2c_client, 0x88);
	dig_T2 = i2c_smbus_read_word_data(bmp280_i2c_client, 0x8a);
	dig_T3 = i2c_smbus_read_word_data(bmp280_i2c_client, 0x8c);

	i2c_smbus_write_byte_data(bmp280_i2c_client, 0xf5, 5 << 5);
	i2c_smbus_write_byte_data(bmp280_i2c_client, 0xf4, ((5 << 5) | (5 << 2) | (3 << 0)));

	return 0;
reg_error:
	device_destroy(kdt_class, kdt_dev);
device_error:
	class_destroy(kdt_class);
cerror:
	unregister_chrdev_region(kdt_dev, 1);
	return -1;
}

static void __exit kdt_module_exit(void)
{
	i2c_unregister_device(bmp280_i2c_client);
	i2c_del_driver(&bmp_driver);
	cdev_del(&kdt_device);
	device_destroy(kdt_class, kdt_dev);
	class_destroy(kdt_class);
	unregister_chrdev_region(kdt_dev, 1);
	pr_info("exit\n");
}

module_init(kdt_module_init);
module_exit(kdt_module_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("KTD <ktd@kdt.com>");
MODULE_DESCRIPTION("kdt i2c");
MODULE_VERSION("1.0.0");