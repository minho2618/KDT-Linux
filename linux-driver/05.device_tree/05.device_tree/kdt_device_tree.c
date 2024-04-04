#include <linux/module.h>
#include <linux/init.h>
#include <linux/mod_devicetable.h>
#include <linux/property.h>
#include <linux/platform_device.h>
#include <linux/of_device.h>

static int device_tree_probe(struct platform_device *pdev);
static int dt_remove(struct platform_device *pdev);

static struct of_device_id kdt_driver_ids[] = {
	{
		.compatible = "brcm,kdtdev",
	}, { /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, kdt_driver_ids);

static struct platform_driver kdt_driver = {
	.probe = device_tree_probe,
	.remove = dt_remove,
	.driver = {
		.name = "kdt_device_driver",
		.of_match_table = kdt_driver_ids,
	},
};

static int device_tree_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	const char *str;
	int kdt_value, ret;

	pr_info("device_tree_probe\n");

	if (!device_property_present(dev, "int_value")) {
		pr_err("device_tree_probe - Error!\n");
		return -1;
	}

	if (!device_property_present(dev, "string_value")) {
		pr_err("device_tree_probe - Error!\n");
		return -1;
	}

	ret = device_property_read_string(dev, "string_value", &str);
	if (ret) {
		pr_err("device_tree_probe - Error!\n");
		return -1;
	}
	pr_info("device_tree_probe - string_value: %s\n", str);

	ret = device_property_read_u32(dev, "int_value", &kdt_value);
	if (ret) {
		pr_info("device_tree_probe - Error\n");
		return -1;
	}
	pr_info("device_tree_probe - kdt_value: %d\n", kdt_value);

	return 0;
}

static int dt_remove(struct platform_device *pdev)
{
	pr_info("device_tree_probe - Now I am in the remove function\n");

	return 0;
}

static int __init kdt_module_init(void)
{
	pr_info("KDT Device tree 예제 입니다.!!\n");
	pr_info("드라이버를 로딩합니다....\n");

	if (platform_driver_register(&kdt_driver)) {
		pr_err("device_tree_probe - 에러! \n");
		return -1;
	}

	return 0;
}

static void __exit kdt_module_exit(void)
{
	pr_info("device_tree_probe - 언로드");
	platform_driver_unregister(&kdt_driver);
}

module_init(kdt_module_init);
module_exit(kdt_module_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("KTD <ktd@kdt.com>");
MODULE_DESCRIPTION("kdt device tree module");
MODULE_VERSION("1.0.0");
