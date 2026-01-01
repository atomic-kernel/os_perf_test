#include "common.h"

int do_test_init(const size_t loop)
{
	return 0;
}

void do_test(const size_t loop)
{
	size_t i;
	DEFINE_RAW_SPINLOCK(lock);

	for (i = 0; i < loop; ++i) {
		__asm__ volatile ("":"+m"(lock)::);
		raw_spin_lock(&lock);
		__asm__ volatile ("":"+m"(lock)::);
		raw_spin_unlock(&lock);
	}
}

void do_test_deinit(const size_t loop)
{
}
