#include<linux/kernel.h>
#include<linux/init.h>
#include<linux/module.h>

extern int shared_count;
void kdt_shared_func(void);

static int __init kdt_module_init(void)
{
    pr_info("kdt 커널 모듈 \n");
    kdt_shared_func();
    pr_info("%d: 호출되었습니다!\n", shared_count);
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
