#include <linux/module.h>
#define INCLUDE_VERMAGIC
#include <linux/build-salt.h>
#include <linux/elfnote-lto.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

BUILD_SALT;
BUILD_LTO_INFO;

MODULE_INFO(vermagic, VERMAGIC_STRING);
MODULE_INFO(name, KBUILD_MODNAME);

__visible struct module __this_module
__section(".gnu.linkonce.this_module") = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

#ifdef CONFIG_RETPOLINE
MODULE_INFO(retpoline, "Y");
#endif

static const struct modversion_info ____versions[]
__used __section("__versions") = {
	{ 0xa2dd5f9b, "module_layout" },
	{ 0x26e0f5bf, "cdev_del" },
	{ 0x46a4b118, "hrtimer_cancel" },
	{ 0xc1514a3b, "free_irq" },
	{ 0x6091b333, "unregister_chrdev_region" },
	{ 0x5f7139f5, "class_destroy" },
	{ 0xf854e375, "device_destroy" },
	{ 0xfe990052, "gpio_free" },
	{ 0x3c5d543a, "hrtimer_start_range_ns" },
	{ 0x2d0684a9, "hrtimer_init" },
	{ 0x92d5838e, "request_threaded_irq" },
	{ 0xa60c4344, "gpiod_set_debounce" },
	{ 0x7cd33454, "gpiod_to_irq" },
	{ 0x6cc071bc, "gpiod_direction_input" },
	{ 0xf6032ca1, "cdev_add" },
	{ 0xc494b00e, "cdev_init" },
	{ 0x7f03854f, "device_create" },
	{ 0x4dbb193c, "__class_create" },
	{ 0xe3ec2f2b, "alloc_chrdev_region" },
	{ 0x15ba50a6, "jiffies" },
	{ 0x828e22f4, "hrtimer_forward" },
	{ 0x8da6585d, "__stack_chk_fail" },
	{ 0x10eb9a8a, "gpiod_direction_output_raw" },
	{ 0x47229b5c, "gpio_request" },
	{ 0x656e4a6e, "snprintf" },
	{ 0xe1194d5, "hrtimer_try_to_cancel" },
	{ 0x131ae166, "gpiod_set_raw_value" },
	{ 0xb51fd214, "gpio_to_desc" },
	{ 0x92997ed8, "_printk" },
};

MODULE_INFO(depends, "");


MODULE_INFO(srcversion, "1155FE9C1D7AE0EFD54683C");
