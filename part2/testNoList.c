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
#define __NR_cs3013_syscall2 378

long testCall2 ( void) {
	struct processinfo info;
	long val = (long) syscall(__NR_cs3013_syscall2, &info);

	if (val == -1) {
	    //if error return
		return val;
	}

	printf("\nUID is %d\n", info.uid);
	printf("PID is %d\n", info.pid);
	printf("Parent PID is %d\n", info.parent_pid);
	printf("Yougest Child PID is %d\n", info.youngest_child);
	printf("Younger Sibling PID is %d\n", info.younger_sibling);
	printf("Older Sibling PID is %d\n", info.older_sibling);
	printf("State is %ld\n", info.state);
	printf("Start time is %lld\n", info.start_time);
	printf("System time is %lld\n", info.sys_time);
	printf("User time is %lld\n", info.user_time);
	printf("Child User time is %lld\n", info.cutime);
	printf("Child system time is %lld\n\n", info.cstime);

	return val;
}


int main () {


	printf("The return values of putting NULL into cs3013_syscall2:\n");
	printf("cs3013_syscall2: %ld\n", testCall2());

	
	return 0;
}