#ifndef _GNU_SOURCE
#define _GNU_SOURCE 1
#endif

#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stddef.h>
#include <stdatomic.h>

#include <sched.h>
#include <unistd.h>

#include "gettime.h"

static inline void bind_cpu(const int cpu)
{
	cpu_set_t set;

	CPU_ZERO(&set);
	CPU_SET(cpu, &set);

	assert(sched_setaffinity(0, sizeof(cpu_set_t), &set) == 0);
}

extern void do_test(unsigned long long loop);

void __attribute__((weak)) do_test_init(unsigned long long loop)
{
	(void)loop;
}

void __attribute__((weak)) do_test_deinit(unsigned long long loop)
{
	(void)loop;
}

int main(int argc, char *argv[])
{
	if (argc != 3) {
		printf("usage: bindcpu, looptime\n");
		return -1;
	}

	const int cpu = atoi(argv[1]);
	const unsigned long long loop = (unsigned long long)atoll(argv[2]);

	if (cpu == -1)
		printf("not binding cpu\n");
	else
		bind_cpu(cpu);

	if (getres_ns() > 10000)
		printf("WARNING: res > 10us, maybe hires is not enabled\n");

	printf("loop time %llu, sleep 1s and start to test\n", loop);
	
	sleep(1);

	// 防止代码段缺页异常
	do_test(1);

	const unsigned long long start_time = gettime_ns();
	const unsigned long long arch_start_time = arch_gettime();

	do_test(loop);

	const unsigned long long arch_end_time = arch_gettime();
	const unsigned long long end_time = gettime_ns();
	const unsigned long long cost_time = end_time - start_time;
	const unsigned long long arch_cost_time = arch_end_time - arch_start_time;

	printf("cost time: %llu.%09llu s, %llu ns, avg %Lf ns\n", cost_time / 1000000000, cost_time % 1000000000, cost_time, (long double)cost_time / loop);
	printf("cost archtime: %llu, avg %Lf\n", arch_cost_time, (long double)arch_cost_time / loop);

	do_test_deinit(loop);

	return 0;
}
