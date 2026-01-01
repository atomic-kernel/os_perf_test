#include <linux/types.h>
#include <linux/sched.h>

int do_test_init(size_t loop);
void do_test(size_t loop);
void do_test_deinit(size_t loop);
uint32_t simple_random(void);
int ko_wake_up_state(struct task_struct *p, int state);
