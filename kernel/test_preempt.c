#include <linux/printk.h>

#include "common.h"

int do_test_init(const size_t loop)
{
#if defined(CONFIG_PREEMPT_COUNT) && (defined(CONFIG_DEBUG_PREEMPT) || defined(CONFIG_TRACE_PREEMPT_TOGGLE))
	return 0;
#else
	pr_err("CONFIG_PREEMPT_CONT/CONFIG_DEBUG_PREEMPT/CONFIG_TRACE_PREEMPT_TOGGLE is not set, do not need to test!\n");
	return -1;
#endif
}

void do_test(const size_t loop)
{
#if defined(CONFIG_PREEMPT_COUNT) && (defined(CONFIG_DEBUG_PREEMPT) || defined(CONFIG_TRACE_PREEMPT_TOGGLE))
	size_t i;

	for (i = 0; i < loop; ++i) {
		preempt_disable();
		preempt_enable();
	}
#endif
}

void do_test_deinit(const size_t loop)
{
}
