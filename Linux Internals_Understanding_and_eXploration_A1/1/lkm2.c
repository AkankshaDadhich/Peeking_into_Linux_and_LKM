#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/sched/signal.h>
#include <linux/sched.h>
 
 
struct task_struct *task;       
struct task_struct *task_child;      
struct list_head *list;        //  Structure needed to iterate through the list in each task->children struct      
// default initilized
static int pid = 1;
// take pid as a parameter
module_param(pid, int, 0); 


int iterate_init(void)                    
{
    printk(KERN_ALERT "%s","LOADING MODULE\n");   
   
    task = pid_task(find_get_pid(pid), PIDTYPE_PID); // getting the task from pid
    if (task == NULL) {
       printk(KERN_ERR "Target process with PID %d not found\n", pid);
        return -1; 
    }  // prints child pid name and its state
        printk(KERN_INFO "\nPARENT PID: %d PROCESS: %s STATE: %ld",task->pid, task->comm, task_state_index(task));
        // iterating through each child of this process using list_for_each
        list_for_each(list, &task->children){        
                            
         task_child = list_entry( list, struct task_struct, sibling );    // this is to get the task struct of the child from the current list entry

     
        printk(KERN_INFO "\nCHILD OF %s[%d] PID: %d PROCESS: %s STATE: %ld",task->comm, task->pid,
              task_child->pid, task_child->comm, task_state_index(task_child) );
        }
     
 
    return 0;
 
}               
     
void cleanup_exit(void)       
{
 
 
    printk(KERN_INFO "%s","REMOVING MODULE\n");
 
}              
 
module_init(iterate_init);    
module_exit(cleanup_exit);   
 
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("ITERATE THROUGH ALL PROCESSES/CHILD PROCESSES IN THE OS");
MODULE_AUTHOR("Akanksha");