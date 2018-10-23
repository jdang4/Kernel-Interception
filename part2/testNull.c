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

// These values MUST match the syscall_32.tbl modifications:
#define __NR_cs3013_syscall1 377
#define __NR_cs3013_syscall2 378
#define __NR_cs3013_syscall3 379

long testCall1 ( void) {
        return (long) syscall(__NR_cs3013_syscall1);
}

long testCall2 ( void) {
	return (long) syscall(__NR_cs3013_syscall2, NULL);
}

long testCall3 ( void) {
        return (long) syscall(__NR_cs3013_syscall3);
}

int main () {

	
	printf("\nThe return values of the system calls are:\n");
	printf("cs3013_syscall1: %ld\n", testCall1());
	printf("cs3013_syscall2(NULL): %ld\n", testCall2());
	printf("cs3013_syscall3: %ld\n\n", testCall3());
	
	return 0;
}