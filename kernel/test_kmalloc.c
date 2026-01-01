#include "common.h"

#include <linux/mm.h>
#include <linux/printk.h>
#include <linux/types.h>
#include <linux/slab.h>

static uint16_t *random_buf = NULL;

int do_test_init(const size_t loop)
{
	size_t i;

	random_buf = kvmalloc(loop * sizeof(uint16_t), GFP_KERNEL);

	if (!random_buf) {
		pr_err("failed to alloc buf, maybe loop too large!\n");
		return -1;
	}

	for (i = 0; i < loop; ++i)
		random_buf[i] = simple_random() % 4095 + 1;

	return 0;
}

void do_test(const size_t loop)
{
	size_t i;

	for (i = 0; i < loop; ++i) {
		void *const p = kmalloc(random_buf[i], GFP_KERNEL);

		BUG_ON(!p);
		__asm__ volatile ("":"+m"(*(char *)p)::);

		kfree(p);
	}
}

void do_test_deinit(const size_t loop)
{
	kvfree(random_buf);
}
