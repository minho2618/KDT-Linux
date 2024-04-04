#include<linux/kernel.h>
#include<linux/init.h>
#include<linux/module.h>

int shared_count = 0;

void kdt_shared_func(void)
{
    pr_info("Shared function been called!!!\n");
    shared_count++;
}
EXPORT_SYMBOL(kdt_shared_func);
EXPORT_SYMBOL(shared_count);

static int __init kdt_module_init(void)
{
    pr_info("kdt 커널 모듈 \n");
    kdt_shared_func();
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
MODULE_DESCRIPTION("kdt export module");
MODULE_VERSION("1.0.0");
