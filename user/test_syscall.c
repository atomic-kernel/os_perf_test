#include <unistd.h>
#include <sys/syscall.h>

void do_test(const unsigned long long loop)
{
	for (unsigned long long i = 0; i < loop; ++i)
		syscall(__NR_sched_setscheduler, (pid_t)0, (int)-1, NULL);
}
