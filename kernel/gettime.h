#include <linux/sched/clock.h>

static __always_inline
unsigned long long arch_gettime(void)
{
#ifdef __x86_64__
	register uint64_t rax __asm__ ("rax");
	register uint64_t rdx __asm__ ("rdx");

	__asm__ volatile ("rdtsc":"=r"(rax),"=r"(rdx)::);

	return (rdx << 32) | (rax & 0xffffffff);
#elif defined(__aarch64__)
	uint64_t ret;

	__asm__ volatile ("mrs %0, cntvct_el0":"=r"(ret)::);

	return ret;
#elif defined(__arm__)
	uint64_t ret;

	__asm__ volatile("mrrc p15, 1, %Q0, %R0, c14" : "=r" (ret));

	return ret;
#else
#error "not support"
#endif
}
