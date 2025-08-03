// This kernel module I have created is traversing the page table and finding the entry and then calulating the physical address
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/sched/signal.h>
#include<linux/mm_types.h>
#include <linux/mm.h>
#include<linux/init.h>

#include <asm/pgtable.h>
#include <linux/moduleparam.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Akanksha");

struct task_struct *task;       
static int pid = 1;
module_param(pid, int, 0); 
static unsigned long v_addr = 0;  
module_param(v_addr, ulong, 0);
static int __init lkm3_init(void)
{
    pgd_t *pgd;
    pud_t *pud;
    pmd_t *pmd;
    p4d_t* p4d;
    pte_t *pte;
    unsigned long paddr;
    printk(KERN_ALERT "%s", "LOADING MODULE\n");
    // finding the task struct of the pid
    task = pid_task(find_get_pid(pid), PIDTYPE_PID);
    if(!task){
        printk(KERN_ALERT "No such pid\n");
        return 0;
    }
    down_read(&task->mm->mmap_lock);
    // acquires lock
    // page table global entry 
    pgd = pgd_offset(task->mm, v_addr);
    if (pgd_none(*pgd) || pgd_bad(*pgd)) {
        printk(KERN_ALERT "No pgd\n");
        return -1;
    }
    
    p4d = p4d_offset(pgd, v_addr);
        if (p4d_none(*p4d) || p4d_bad(*p4d))
        return 0;
     
    
    // upper page directory 
    pud = pud_offset(p4d, v_addr);
    if (pud_none(*pud) || pud_bad(*pud)) {
        printk(KERN_ALERT "No pud\n");
        return -1;
    }

 
    // this is middle page directory
    pmd = pmd_offset(pud, v_addr);
    if (pmd_none(*pmd) || pmd_bad(*pmd)) {
        printk(KERN_ALERT "No pmd\n");
        return -1;
    }
    // finding the page table entry 
    pte = pte_offset_map(pmd, v_addr);
    if (!pte || !pte_present(*pte)) {
        printk(KERN_ALERT "No pte\n");
        return -1;
    }
    // The below lines are for finfing the virtual address.
    paddr = pte_pfn(*pte) << PAGE_SHIFT;
    paddr |= v_addr & ~PAGE_MASK;
    printk(KERN_ALERT "Physical address: ox%lx\n", paddr);

    up_read(&task->mm->mmap_lock);

    //paddr = virt_to_phys(v_addr);
    printk(KERN_ALERT "pid=%d\n",pid);
    printk(KERN_ALERT "v_addr=0x%lx\n", v_addr);
   // printk(KERN_ALERT "pa=0x%lx\n", paddr);


   
    return 0;
}

static void __exit lkm3_exit(void)
{
    pr_info("lkm3 module unloaded\n");
}

module_init(lkm3_init);
module_exit(lkm3_exit);
