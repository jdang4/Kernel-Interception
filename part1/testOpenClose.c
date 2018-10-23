// Jonathan Dang and Clay Oshiro-Leavitt
// WPI Username: jdang and coshiroleavitt 

#include <sys/syscall.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

// These values MUST match the syscall_32.tbl modifications:
#define __NR_cs3013_syscall1 377
#define __NR_cs3013_syscall2 378
#define __NR_cs3013_syscall3 379

long testCall1 ( void) {
        return (long) syscall(__NR_cs3013_syscall1);
}

long testCall2 ( void) {
        int fd1, fd2;
        int uid1, uid2;

        uid1 = getuid();
        const char *filename1 = "test.txt";
        printf("Testing Opening Call and Closing Call:\n");
        fd1 = open(filename1, 0, 0);

        if(fd1 == -1) {
        	printf("ERROR when opening\n");
        }

        else {
        	printf("User %d is opening file: %s\n", uid1, filename1);
        }

        if(close(fd1) != -1) {
        	printf("User %d is closing file descriptor: %d\n\n", uid1, fd1);
        }

        uid2 = getuid();
        const char *filename2 = "syscalls.h";
        fd2 = open(filename2, 0, 0);

        if(fd2 == -1) {
        	printf("ERROR when opening\n");
        }

        else {
        	printf("User %d is opening file: %s\n", uid2, filename2);
        }

        if(close(fd2) != -1) {
        	printf("User %d is closing file descriptor: %d\n\n", uid2, fd2);
        }

        return 0;
}

long testCall3 ( void) {
        return (long) syscall(__NR_cs3013_syscall3);
}

int main () {
        printf("The return values of the system calls are:\n");
        printf("cs3013_syscall1: %ld\n\n", testCall1());
        printf("cs3013_syscall2: %ld\n", testCall2());
        printf("cs3013_syscall3: %ld\n", testCall3());
        return 0;
}