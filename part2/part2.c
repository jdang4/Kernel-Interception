// Jonathan Dang and Clay Oshiro-Leavitt
// WPI Username: jdang and coshiroleavitt 

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/syscalls.h>
#include <linux/sched.h>
#include <asm/current.h>
#include <linux/time.h>
#include <asm-generic/cputime.h>
#include <linux/cred.h>
#include <linux/list.h>
#include <linux/unistd.h>
#include <linux/types.h>
#include "processInfo.h"


unsigned long **sys_call_table;

asmlinkage long (*ref_sys_cs3013_syscall2)(void);

asmlinkage long new_sys_cs3013_syscall2(struct processinfo *info) {

	struct task_struct *task_info = current; // puts all the process info of current process into task_struct

	//struct processinfo *temp = malloc(sizeof(struct processinfo));
	struct processinfo kinfo;
	struct list_head *pos; // stores the pointer to the head of the list (I believe)

	pid_t youngestChild; // stores the pid of the youngest child
	pid_t youngerSibling; // stores the pid of the younger sibling
	pid_t olderSibling; // stores the pid of the older sibling

	long long userChild_time; // the user time in the child
	long long systemChild_time; // the system time in the child

	long long temp_youngestChild_startTime; // a temp value of determining the youngest child

	long long YS_difference; // used to find the start time that is nearest to the current process start time (Younger Sibling)
	long long OS_difference; // used to find the start time that is nearest to the current process start time (Oldest Sibling)

	long long startTime;

	userChild_time = 0;
	systemChild_time = 0;

	YS_difference = 0;
	OS_difference = 0;

	temp_youngestChild_startTime = 0;

	// INITALIZING THE PROCESS INFO STRUCT
	kinfo.state = task_info->state; // gets the current state of the process
	kinfo.pid = task_info->pid; // gets the pid of the process
	kinfo.parent_pid = task_info->parent->pid; // gets the pid of the parent process
	//kinfo.uid = current_uid().val; // gets the current uid
	kinfo.uid = current_uid().val;

	// gets the current process start time (in nanoseconds)
	kinfo.start_time = task_info->start_time;

	kinfo.user_time = cputime_to_usecs(task_info->utime); // gets the CPU time in user mode (microseconds)
	
	kinfo.sys_time = cputime_to_usecs(task_info->stime); // gets the CPU time in system mode (micoseconds)


	/***** FINDING THE YOUNGEST CHILD *****/

	// if the children list is not empty
	if (!list_empty(&(task_info->children))) {

		// iterates through each element in the children list
		list_for_each(pos, &(task_info->children)) {

			// stores the information of the current element in children list
			struct task_struct *tmp = list_entry(pos, struct task_struct, sibling);

			userChild_time += cputime_to_usecs(tmp->utime); // calculating the user time of children (microseconds)
			printk(KERN_INFO "U Child: %lld\n", userChild_time);
			
			systemChild_time += cputime_to_usecs(tmp->stime); // calculating the system time of children (microseconds)
			
			startTime = tmp->start_time; // gets the start time of the current element in nanoseconds
			printk(KERN_INFO "Start Time: %lld || PID: %d\n", startTime, tmp->pid);
			
			// finding the one with the latest start time (highest start time)
			if (startTime > temp_youngestChild_startTime) {
				temp_youngestChild_startTime = startTime; 
				youngestChild = tmp->pid;
			}
		}
	}

	// if the children list is empty then set the youngestChild field to -1
	else {
		youngestChild = -1;
	} 
	
	/***** FINDING THE YOUNGESR SIBLING AND OLDER SIBLING *****/
	
	list_for_each(pos, &(task_info->sibling)) {

		struct task_struct *tmp = list_entry(pos, struct task_struct, sibling);
		long long c_startTime = tmp->start_time; // stores the start time of the current element in sibling

		// finding the younger sibling
		if ((c_startTime > kinfo.start_time) && 
			((YS_difference == 0) || (c_startTime - kinfo.start_time) < YS_difference)) {

			YS_difference = c_startTime - kinfo.start_time;
			youngerSibling = tmp->pid;
		}

		// finding the older sibling
		else if ((c_startTime < kinfo.start_time) &&
			((OS_difference == 0) || (kinfo.start_time - c_startTime) < OS_difference)) {

			OS_difference = kinfo.start_time - c_startTime;
			olderSibling = tmp->pid;
		}
	}

	// if there was no younger sibling
	if (YS_difference == 0) {
		youngerSibling = -1;
	}

	// if there was no older sibling
	if (OS_difference == 0) {
		olderSibling = -1;
	}

	// finishing up initializing the processinfo struct
	kinfo.cutime = userChild_time;
	kinfo.cstime = systemChild_time;
	kinfo.youngest_child = youngestChild;
	kinfo.younger_sibling = youngerSibling;
	kinfo.older_sibling = olderSibling;

	if (info == NULL) {
		return -1;
	}

	if (copy_to_user(info, &kinfo, sizeof(kinfo))) {
		return EFAULT;
	}

	return 0;
}

static unsigned long **find_sys_call_table(void) {
  unsigned long int offset = PAGE_OFFSET;
  unsigned long **sct;
  
  while (offset < ULLONG_MAX) {
    sct = (unsigned long **)offset;

    if (sct[__NR_close] == (unsigned long *) sys_close) {
      printk(KERN_INFO "Interceptor: Found syscall table at address: 0x%02lX",
                (unsigned long) sct);
      return sct;
    }
    
    offset += sizeof(void *);
  }
  
  return NULL;
}

static void disable_page_protection(void) {
  /*
    Control Register 0 (cr0) governs how the CPU operates.

    Bit #16, if set, prevents the CPU from writing to memory marked as
    read only. Well, our system call table meets that description.
    But, we can simply turn off this bit in cr0 to allow us to make
    changes. We read in the current value of the register (32 or 64
    bits wide), and AND that with a value where all bits are 0 except
    the 16th bit (using a negation operation), causing the write_cr0
    value to have the 16th bit cleared (with all other bits staying
    the same. We will thus be able to write to the protected memory.

    It's good to be the kernel!
   */
  write_cr0 (read_cr0 () & (~ 0x10000));
}

static void enable_page_protection(void) {
  /*
   See the above description for cr0. Here, we use an OR to set the 
   16th bit to re-enable write protection on the CPU.
  */
  write_cr0 (read_cr0 () | 0x10000);
}

static int __init interceptor_start(void) {
  /* Find the system call table */
  if(!(sys_call_table = find_sys_call_table())) {
    /* Well, that didn't work. 
       Cancel the module loading step. */
    return -1;
  }
  
  /* Store a copy of all the existing functions */
  ref_sys_cs3013_syscall2 = (void *)sys_call_table[__NR_cs3013_syscall2];

  /* Replace the existing system calls */
  disable_page_protection();

  sys_call_table[__NR_cs3013_syscall2] = (unsigned long *)new_sys_cs3013_syscall2;
  
  enable_page_protection();
  
  /* And indicate the load was successful */
  printk(KERN_INFO "Loaded interceptor!");

  return 0;
}

static void __exit interceptor_end(void) {
  /* If we don't know what the syscall table is, don't bother. */
  if(!sys_call_table)
    return;
  
  /* Revert all system calls to what they were before we began. */
  disable_page_protection();
  sys_call_table[__NR_cs3013_syscall2] = (unsigned long *)ref_sys_cs3013_syscall2;
  enable_page_protection();

  printk(KERN_INFO "Unloaded interceptor!");
}

MODULE_LICENSE("GPL");
module_init(interceptor_start);
module_exit(interceptor_end);
