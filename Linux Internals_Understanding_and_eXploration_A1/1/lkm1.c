#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/sched/signal.h>
#include <linux/sched.h>
 
 
struct task_struct *task;        //   Structure defined in sched.h for tasks/processes   
struct task_struct *task_child;  //   Structure needed to iterate through task children    

 
int iterate_init(void)                  
{
    printk(KERN_INFO "%s","LOADING MODULE\n");    \
     
    // Iterate over all processes using for_each_process() MACRO located in linux\sched\signal.h   
    for_each_process(task) {      
        // Check if the process is in a running or runnable state I have used task_state_index to check where 0  is  Running or runnable (on run queue)
        // #define TASK_INTERRUPTIBLE		0x00000001 this is code written in #include <linux/sched.h> of bootlin link
 
        //#define TASK_UNINTERRUPTIBLE		0x00000002 

        if (task_state_index(task) == 0 ) {
           
            printk(KERN_INFO "PID: %d PROCESS Running: %s", task->pid, task->comm);
        }
    }
     
 
    return 0;
 
}                
     
void cleanup_exit(void)   // run when module exists   
{
 

    printk(KERN_INFO "%s","REMOVING MODULE\n");
 
}              
 
module_init(iterate_init);    //    Load Module 
module_exit(cleanup_exit);    // Remove Module
 
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("ITERATE THROUGH ALL PROCESSES IN THE OS AND FIND THE PROCESS IN RUNNING STATE");
MODULE_AUTHOR("AKANKSHA");