#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/kobject.h>

static int sysfs_value;

static ssize_t k_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
	size_t len = 0;

	pr_info("show adttr.name: %s\n", attr->attr.name);

	len = scnprintf(buf, PAGE_SIZE, "%d", sysfs_value);

	return len;
}

static ssize_t k_store(struct kobject *kobj, struct kobj_attribute *attr,
					   const char *buf, size_t len)
{
	int val;

	if (!len || !sscanf(buf, "%d", &val))
		return -EINVAL;

	sysfs_value = val;

	pr_info("store value: %d\n", val);

	return len;
}

static struct kobj_attribute test_attr =
	__ATTR(test, 0660, k_show, k_store);

static struct attribute *k_attrs[] = {
	&test_attr.attr,
	NULL,
};

static struct attribute_group k_attr_group = {
	.attrs = k_attrs,
};

static struct kobject *ksys_kobj;

static int __init k_module_init(void)
{
	int err = -1;

	pr_info("k 커널 모듈 \n");

	ksys_kobj = kobject_create_and_add("ksys", kernel_kobj);
	if (!ksys_kobj)
		return -ENOMEM;

	err = sysfs_create_group(ksys_kobj, &k_attr_group);
	if (err)
		kobject_put(ksys_kobj);

	return err;
}

static void __exit k_module_exit(void)
{
	if (ksys_kobj)
		kobject_put(ksys_kobj);

	pr_info("바이!!n");
}

module_init(k_module_init);
module_exit(k_module_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("K <k@k.com>");
MODULE_DESCRIPTION("k sysfs module");
MODULE_VERSION("1.0.0");
