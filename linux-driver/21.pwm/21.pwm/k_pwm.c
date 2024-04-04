#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/pwm.h>

struct pwm_device *pwm0 = NULL;
u32 pwm_on_time = 900000;

static int __init k_module_init(void)
{
	int retval;

	pr_info("k 커널 모듈 \n");

	pwm0 = pwm_request(0, "my-pwm");
	if(pwm0 == NULL) {
		printk("Could not get PWM0!\n");
		return -1;
	}

	pwm_config(pwm0, pwm_on_time, 1000000);
	pwm_enable(pwm0);
	return 0;
}

static void __exit k_module_exit(void)
{
	pwm_disable(pwm0);
	pwm_free(pwm0);

	pr_info("바이!!n");
}

module_init(k_module_init);
module_exit(k_module_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("K <k@k.com>");
MODULE_DESCRIPTION("k initial module");
MODULE_VERSION("1.0.0");
