#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/hrtimer.h>
#include <linux/ktime.h>

#define TIMEOUT_NSEC	1000000000L
#define TIMEOUT_SEC		4

static struct hrtimer k_hrtimer;

enum hrtimer_restart timer_callback(struct hrtimer *timer)
{
	pr_info("callback 함수: jiffies 값: (%ld).\n", jiffies);
	hrtimer_forward_now(timer, ktime_set(TIMEOUT_SEC, TIMEOUT_NSEC));

	return HRTIMER_RESTART;
}

static int __init k_module_init(void)
{
	ktime_t ktime;

	pr_info("k 커널 모듈 \n");

	ktime = ktime_set(TIMEOUT_SEC, TIMEOUT_NSEC);
	hrtimer_init(&k_hrtimer, CLOCK_MONOTONIC, HRTIMER_MODE_REL);
	k_hrtimer.function = &timer_callback;
	hrtimer_start(&k_hrtimer, ktime, HRTIMER_MODE_REL);

	return 0;
}

static void __exit k_module_exit(void)
{
	int retval;

	retval = hrtimer_cancel(&k_hrtimer);
	if (retval)
		pr_info("타이머 제거 \n");

	pr_info("바이!!n");
}

module_init(k_module_init);
module_exit(k_module_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("K <k@k.com>");
MODULE_DESCRIPTION("k initial module");
MODULE_VERSION("1.0.0");
