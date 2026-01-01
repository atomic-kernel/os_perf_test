#include <linux/module.h>
#include <linux/printk.h>
#include <linux/kthread.h>

static bool server_finish_init = false;
static bool server_need_exit = false;

static int cpu1 = -1;
module_param(cpu1, int, 0644);

int server_func(void *const arg)
{
	struct task_struct *const client = arg;

	WRITE_ONCE(server_finish_init, true);
	local_irq_disable();
	while (1) {
		set_special_state(__TASK_STOPPED);
		schedule();
		local_irq_disable();

		if (READ_ONCE(server_need_exit))
			break;

		smp_cond_load_acquire(&client->on_cpu, !VAL);
		wake_up_state(client, __TASK_STOPPED);
	}
	local_irq_enable();

	return 0;
}

int do_test_init(size_t loop)
{
	struct task_struct *server;

	if (cpu1 > 0)
		server = kthread_create_on_cpu(server_func, current, cpu1, "dotest_thread");
	else
		server = kthread_create(server_func, current, "dotest_thread");

	if (IS_ERR(server)) {
		pr_err("failed to create thread!\n");
		return -1;
	}

	memset(&param, 0, sizeof(struct sched_param));
	param.sched_priority = 99;

	if (sched_setscheduler(server, SCHED_FIFO, &param) != 0) {
		pr_err("faied to set scheduler fifo!\n");
		kthread_stop(server);
		return -1;
	}

	wake_up_process(server);

	while (!READ_ONCE(server_finish_init))
		sched_yield();

	return 0;
}

void do_test(const size_t loop)
{
	size_t i;

	local_irq_disable();
	for (i = 0; i < loop; ++i) {
		smp_cond_load_acquire(&client->on_cpu, !VAL);
		wake_up_state(client, __TASK_STOPPED);
		set_special_state(__TASK_STOPPED);
		schedule();
		local_irq_disable();
	}
	local_irq_enable();
}
