#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/timer.h>

static struct timer_list test_timer;

void test_timer_callback(struct timer_list *t)
{
	pr_info("callback 함수: jiffies 값: (%ld).\n", jiffies);
}

static int __init k_module_init(void)
{
	int retval;

	pr_info("k 커널 모듈 \n");

	timer_setup(&test_timer, test_timer_callback, 0);
	pr_info( "현재 jiffies 값: (%ld)\n", jiffies);

	retval = mod_timer(&test_timer, jiffies + msecs_to_jiffies(500));
	if (retval)
		pr_info("타이머 설정 에러\n");

	return 0;
}

static void __exit k_module_exit(void)
{
	int retval;

	retval = del_timer(&test_timer);
	if (retval)
		pr_info("타이머 제거 실패...\n");

	pr_info("바이!!n");
}

module_init(k_module_init);
module_exit(k_module_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("K <k@k.com>");
MODULE_DESCRIPTION("k initial module");
MODULE_VERSION("1.0.0");
