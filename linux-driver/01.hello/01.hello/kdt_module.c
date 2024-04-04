#include<linux/kernel.h>
#include<linux/init.h>
#include<linux/module.h>

static int __init kdt_module_init(void)
{
    pr_info("kdt 커널 모듈 \n");
    return 0;
}

static void __exit kdt_module_exit(void)
{
    pr_info("바이!!n");
}

module_init(kdt_module_init);
module_exit(kdt_module_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("KTD <ktd@kdt.com>");
MODULE_DESCRIPTION("kdt initial module");
MODULE_VERSION("1.0.0");
