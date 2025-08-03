#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>
#include <linux/sched/signal.h>
#include <linux/sched.h>
#include <linux/mm_types.h>
#include <linux/slab.h>

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Finding total vm space ");
MODULE_AUTHOR("AKanksha");

static int process_pid = -1;
struct task_struct *process_task;
struct mm_struct *process_mem;

module_param(process_pid, int, 0);


long unsigned phys_mem_space = 0;
long unsigned virt_mem_space = 0;



int lkm_init(void) {
    printk(KERN_INFO "%s","LOADING MODULE 4\n");
    process_task = pid_task(find_get_pid(process_pid), PIDTYPE_PID);
    if(process_task == NULL) {
        printk(KERN_ALERT " PID not exist\n");
        return 0;
    }
    process_mem = process_task->mm; 
    // found this hiwater_rss in the memory structure of the process. Cross checked with top and found nearly equal space
    phys_mem_space = process_mem->hiwater_rss * PAGE_SIZE;
    // found this total_vm in the memory structure of the process. Cross checked with top virtual space and found nearly equal space
    virt_mem_space = process_mem->total_vm * PAGE_SIZE;
    printk(KERN_INFO "Virtual Space Allocated= %ld kB\n", (virt_mem_space/1024));
    printk(KERN_INFO "Physical Space Allocated= %ld kB\n", (phys_mem_space/1024));
    return 0;
}

void lkm_exit(void) {
    printk(KERN_INFO "%s","MODULE UNLOADING\n");
}

module_init(lkm_init);
module_exit(lkm_exit);