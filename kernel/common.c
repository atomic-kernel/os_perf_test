#include <linux/module.h>
#include <linux/printk.h>
#include <linux/kthread.h>
#include <linux/sched.h>
#include <linux/sched/task.h>

#include "gettime.h"

static int cpu = -1;
module_param(cpu, int, 0644);
static unsigned long loop = 0;
module_param(loop, ulong, 0644);


int do_test_init(size_t);
void do_test(size_t);
void do_test_deinit(size_t);

int test_thread(void *const arg)
{
	unsigned long long start_time;
	unsigned long long arch_start_time;
	unsigned long long end_time;
	unsigned long long arch_end_time;
	unsigned long long cost_time;
	unsigned long long arch_cost_time;

	if (do_test_init(loop)) {
		pr_err("do_test_init failed!\n");
		return -1;
	}

	start_time = sched_clock();
	arch_start_time = arch_gettime();

	do_test(loop);

	arch_end_time = arch_gettime();
	end_time = sched_clock();
	cost_time = end_time - start_time;
	arch_cost_time = arch_end_time - arch_start_time;

	pr_info("cost time: %llu.%09llu s, %llu ns, avg %f ns\n", cost_time / 1000000000, cost_time % 1000000000, cost_time, (double)cost_time / loop);
	pr_info("cost archtime: %llu, avg %f\n", arch_cost_time, (double)arch_cost_time / loop);

	do_test_deinit(loop);
 
	return 0;
}

static int __init do_module_init(void)
{
	int ret;
	struct task_struct *p;
	struct sched_param param;

	// 创建新线程，防止被用户态影响：
	//   init_module 函数执行主体为用户态insmod任务
	//   kthread_create 创建线程fork自内核线程"create_thread"
	if (cpu > 0)
		p = kthread_create_on_cpu(test_thread, NULL, cpu, "dotest_thread");
	else
		p = kthread_create(test_thread, NULL, "dotest_thread");

	if (IS_ERR(p)) {
		pr_err("failed to create thread!\n");
		return -1;
	}

	memset(&param, 0, sizeof(struct sched_param));
	param.sched_priority = 99;

	if (sched_setscheduler(p, SCHED_FIFO, &param) != 0) {
		pr_err("faied to set scheduler fifo!\n");
		kthread_stop(p);
		return -1;
	}

	get_task_struct(p);
	wake_up_process(p);

	ret = kthread_stop(p);
	pr_err("test thread ret: %d\n", ret);
	put_task_struct(p);
	return 0;
}
 
static void __exit do_cleanup_module(void) 
{ 
	pr_info("finish\n");
}

uint32_t simple_random(void)
{
	static uint32_t seed = 0;
	uint32_t ret;

	seed = ((uint64_t)1103515245 * seed + 12345) % ((uint64_t)1 << 31);
	ret = (seed >> 16) & 0x7fff;
	seed = ((uint64_t)1103515245 * seed + 12345) % ((uint64_t)1 << 31);
	ret |= ((seed >> 15) & 0xffff) << 15;

	return ret;
}

module_init(do_module_init);
module_exit(do_cleanup_module);
 
MODULE_LICENSE("GPL");
