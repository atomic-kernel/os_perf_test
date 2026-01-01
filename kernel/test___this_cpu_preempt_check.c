#include <linux/printk.h>

#include "common.h"

int do_test_init(const size_t loop)
{
#ifdef CONFIG_DEBUG_PREEMPT
	return 0;
#else
	pr_err("CONFIG_DEBUG_PREEMPT is not set, do not need to test!\n");
	return -1;
#endif
}

void do_test(const size_t loop)
{
#ifdef CONFIG_DEBUG_PREEMPT
	size_t i;

	preempt_disable();
	for (i = 0; i < loop; ++i)
		__this_cpu_preempt_check("");
	preempt_enable();
#endif
}

void do_test_deinit(const size_t loop)
{
}
