#include<linux/kernel.h>
#include<linux/init.h>
#include<linux/module.h>
#include <linux/moduleparam.h>

static char *kdt_str = "hello";
static int kdt_int = 1;

module_param(kdt_str, charp, S_IRUGO);
module_param(kdt_int, int, S_IRUGO);

MODULE_PARM_DESC(kdt_str,"this is my char pointer variable");
MODULE_PARM_DESC(kdt_int,"this is my int variable");

static int __init param_init(void) {
    pr_info("kdt_str 값: %s\n", kdt_str);
    pr_info("kdt_int 값: %d\n", kdt_int);
    return 0;
}

static int __init kdt_module_init(void)
{
    pr_info("kdt 커널 모듈 \n");
    param_init();
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
MODULE_DESCRIPTION("kdt param module");
MODULE_VERSION("1.0.0");
