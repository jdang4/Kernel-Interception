// Jonathan Dang and Clay Oshiro-Leavitt
// WPI Username: jdang and coshiroleavitt 

#include <sys/syscall.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "processInfo.h"
#include <sys/time.h>
#include <sys/resource.h>

// These values MUST match the unistd_32.h modifications:
//#define __NR_cs3013_syscall1 377
#define __NR_cs3013_syscall2 378
//#define __NR_cs3013_syscall3 379


long testCall2 ( void) {
	struct processinfo info;
	long val = (long) syscall(__NR_cs3013_syscall2, &info);

	if (val == -1) {
	    //if error return
		return val;
	}

	printf("\nProcess Info of Current Process:\n");
	printf("State: %ld\n", info.state);
	printf("PID: %d\n", info.pid);
	printf("Parent PID: %d\n", info.parent_pid);
	printf("Yougest Child PID: %d\n", info.youngest_child);
	printf("Younger Sibling PID: %d\n", info.younger_sibling);
	printf("Older Sibling PID: %d\n", info.older_sibling);
	printf("UID: %d\n", info.uid);
	printf("Start time: %lld\n", info.start_time);
	printf("CPU time in User Mode: %lld\n", info.user_time); 
	printf("CPU time in System Mode: %lld\n", info.sys_time);
	printf("User time of Children: %lld\n", info.cutime);
	printf("System time of Children: %lld\n\n", info.cstime);

	return val;
}

// adds a CPU Intensive to make CPU work longer
void CPU_consumer(void) {
	long long i;
	long long j = 0;
	for (i = 0; i < 900000; i++) {
		j++;
	}

}

int main () {

	int pid1, pid2;

    pid1 = fork();

	if (pid1 == 0) {
		CPU_consumer();
		return 0;
	}
	
	pid2 = fork();

	if (pid2 == 0) {
		CPU_consumer();
		return 0;
	}

	sleep(1);
	//printf("The return value of the cs3013_syscall2 is:\n");
	printf("cs3013_syscall2 returned: %ld\n", testCall2());
	
	//wait for children to terminate
	wait(0);
	wait(0);
	
	return 0;
}