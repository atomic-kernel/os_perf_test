#include "common.h"

int do_test_init(const size_t loop)
{
	return 0;
}

void do_test(const size_t loop)
{
	size_t i;
	unsigned long flags;

	for (i = 0; i < loop; ++i) {
		local_irq_save(flags);
		local_irq_restore(flags);
	}
}

void do_test_deinit(const size_t loop)
{
}
