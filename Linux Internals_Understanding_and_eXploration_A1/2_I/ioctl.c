#include <linux/cdev.h>
#include <linux/fcntl.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/semaphore.h>
#include <linux/slab.h>
#include <linux/types.h>
#include <asm/atomic.h>
#include <asm/io.h>
#include <asm/uaccess.h>
#include <linux/delay.h>
#include <linux/cdev.h>	
#include <linux/uaccess.h>
#include<linux/mm_types.h>
#include <linux/mm.h>
#include<linux/init.h>
#include <linux/io.h>
#include <asm/io.h>

#include <asm/pgtable.h>

#include "ioctl_dev.h"
#include "ioctl.h"
//used github code as a reference which was given in the assignment https://github.com/pokitoz/ioctl_driver
/* store the major number extracted by dev_t */
int ioctl_d_interface_major = 42;
int ioctl_d_interface_minor = 5;

#define DEVICE_NAME "ioctl"
char* ioctl_d_interface_name = DEVICE_NAME;

ioctl_d_interface_dev ioctl_d_interface;

// file operations
struct file_operations ioctl_d_interface_fops = {
	.owner = THIS_MODULE,
	.read = NULL,
	.write = NULL,
	.open = ioctl_d_interface_open,
	.unlocked_ioctl = ioctl_d_interface_ioctl,
	.release = ioctl_d_interface_release
};

/* Private API */
static int ioctl_d_interface_dev_init(ioctl_d_interface_dev* ioctl_d_interface);
static void ioctl_d_interface_dev_del(ioctl_d_interface_dev* ioctl_d_interface);
static int ioctl_d_interface_setup_cdev(ioctl_d_interface_dev* ioctl_d_interface);
static int ioctl_d_interface_init(void);
static void ioctl_d_interface_exit(void);

// used to pass between userspace and kernel space or vice versa
struct ioctl_data {
    unsigned long virt_addr;
    pid_t process_id;
	unsigned long phys_addr;
};
// used this to get physical address given a virtual address and pid same as lkm3
static int ioctl_vir_to_phy( unsigned long v_addr  , int pid)
{

	pgd_t *pgd;
    pud_t *pud;
    pmd_t *pmd;
    p4d_t* p4d;
    pte_t *pte;
    unsigned long paddr;
    struct task_struct *task;       



    printk(KERN_ALERT "%s", "LOADING MODULE\n");

    task = pid_task(find_get_pid(pid), PIDTYPE_PID);
    if(!task){
        printk(KERN_ALERT "No such pid\n");
        return 0;
    }
    down_read(&task->mm->mmap_lock);
    // acquires lock
    pgd = pgd_offset(task->mm, v_addr);
    if (pgd_none(*pgd) || pgd_bad(*pgd)) {
        printk(KERN_ALERT "No pgd\n");
        return -1;
    }
    
    p4d = p4d_offset(pgd, v_addr);
        if (p4d_none(*p4d) || p4d_bad(*p4d))
        return 0;


    pud = pud_offset(p4d, v_addr);
    if (pud_none(*pud) || pud_bad(*pud)) {
        printk(KERN_ALERT "No pud\n");
        return -1;
    }

 

    pmd = pmd_offset(pud, v_addr);
    if (pmd_none(*pmd) || pmd_bad(*pmd)) {
        printk(KERN_ALERT "No pmd\n");
        return -1;
    }

    pte = pte_offset_map(pmd, v_addr);
    if (!pte || !pte_present(*pte)) {
        printk(KERN_ALERT "No pte\n");
        return -1;
    }

    paddr = pte_pfn(*pte) << PAGE_SHIFT;
    paddr |= v_addr & ~PAGE_MASK;
    printk(KERN_ALERT "Physical address: ox%lx\n", paddr);

    up_read(&task->mm->mmap_lock);

    //paddr = virt_to_phys(v_addr);
    printk(KERN_ALERT "pid=%d\n",pid);
    printk(KERN_ALERT "v_addr=0x%lx\n", v_addr);
   // printk(KERN_ALERT "pa=0x%lx\n", paddr);


   
    return paddr;
}


//initialization function
static int ioctl_d_interface_dev_init(ioctl_d_interface_dev * ioctl_d_interface)
{
	int result = 0;
	memset(ioctl_d_interface, 0, sizeof(ioctl_d_interface_dev));
	atomic_set(&ioctl_d_interface->available, 1);
	sema_init(&ioctl_d_interface->sem, 1);

	return result;
}

static void ioctl_d_interface_dev_del(ioctl_d_interface_dev * ioctl_d_interface) {}

// setup of driver function like initilization of driver and let the kernel know that the driver exists using cdev_add
static int ioctl_d_interface_setup_cdev(ioctl_d_interface_dev * ioctl_d_interface)
{
  int error = 0;
	dev_t devno = MKDEV(ioctl_d_interface_major, ioctl_d_interface_minor);

	cdev_init(&ioctl_d_interface->cdev, &ioctl_d_interface_fops);
	
	ioctl_d_interface->cdev.owner = THIS_MODULE;
	ioctl_d_interface->cdev.ops = &ioctl_d_interface_fops;
	error = cdev_add(&ioctl_d_interface->cdev, devno, 1);

	return error;
}
// all the above functions used here 
static int ioctl_d_interface_init(void)
{
	dev_t           devno = 0;
	int             result = 0;

	
	 ioctl_d_interface_dev_init(&ioctl_d_interface);

    /* register char device using register_chrdev_region */
    result = register_chrdev_region(MKDEV(42, ioctl_d_interface_minor), 1, ioctl_d_interface_name);
    if (result < 0) {
        printk(KERN_WARNING "ioctl_d_interface: can't register major number\n");
        goto fail;
    }

    result = ioctl_d_interface_setup_cdev(&ioctl_d_interface);
    if (result < 0) {
        printk(KERN_WARNING "ioctl_d_interface: error %d adding ioctl_d_interface\n", result);
        goto fail_unregister;
    }
	printk(KERN_INFO "ioctl_d_interface: module loaded\n");
	return 0;

fail_unregister:
    unregister_chrdev_region(MKDEV(ioctl_d_interface_major, ioctl_d_interface_minor), 1);

fail:
	ioctl_d_interface_exit();
	return result;

	
}

static void ioctl_d_interface_exit(void)
{
	dev_t devno = MKDEV(ioctl_d_interface_major, ioctl_d_interface_minor);

	cdev_del(&ioctl_d_interface.cdev);
	unregister_chrdev_region(devno, 1);
	ioctl_d_interface_dev_del(&ioctl_d_interface);

	printk(KERN_INFO "ioctl_d_interface: module unloaded\n");
}

/* Public API */
int ioctl_d_interface_open(struct inode *inode, struct file *filp)
{
	ioctl_d_interface_dev *ioctl_d_interface;

	ioctl_d_interface = container_of(inode->i_cdev, ioctl_d_interface_dev, cdev);
	filp->private_data = ioctl_d_interface;

	if (!atomic_dec_and_test(&ioctl_d_interface->available)) {
		atomic_inc(&ioctl_d_interface->available);
		printk(KERN_ALERT "open ioctl_d_interface : the device has been opened by some other device, unable to open lock\n");
		return -EBUSY;		/* already open */
	}

	return 0;
}

int ioctl_d_interface_release(struct inode *inode, struct file *filp)
{
	ioctl_d_interface_dev *ioctl_d_interface = filp->private_data;
	atomic_inc(&ioctl_d_interface->available);	/* release the device */
	return 0;
}
// writing to the physical address
void write_to_physical_memory(unsigned long phys_addr, uint8_t value) {
    void __iomem *ptr;

    // Map the physical address to kernel virtual address
    ptr = ioremap(phys_addr, sizeof(uint8_t));
    if (!ptr) {
        printk(KERN_ERR "Failed to map physical address\n");
        return;
    }

    // Write the value
    writeb(value, ptr);

    // Unmap the address
    iounmap(ptr);
}
// main ioctl functionality function which uses switch to compare
long ioctl_d_interface_ioctl(struct file *cd, unsigned int cmd, unsigned long arg)
{   struct ioctl_data data;
	switch (cmd) {
        case IOCTL_VIR_TO_PHY:
			
			printk(KERN_INFO "<%s> ioctl: IOCTL_VIR_TO_PHY\n", DEVICE_NAME);
            if (copy_from_user(&data, (struct ioctl_data *)arg, sizeof(struct ioctl_data))) {
                return -EFAULT;
            }

			data.phys_addr  = ioctl_vir_to_phy(data.virt_addr , data.process_id);
            printk(KERN_INFO "The Physical address of the corresponding virtal address %lx of currently running process is %lx\n" , data.virt_addr , data.phys_addr );
			if (copy_to_user((unsigned long *)arg, &data, sizeof(struct ioctl_data)))  {
                return -1;
            }
			
			break;
        case IOCTL_WRITE_TO_PHY:
            
            printk(KERN_INFO "<%s> ioctl: IOCTL_WRITE_TO_PHY\n", DEVICE_NAME);
            struct write_data write_data;
            if (copy_from_user(&write_data, (struct write_data *)arg, sizeof(struct write_data))) {
                return -1;
            }
            write_to_physical_memory(write_data.phys_addr, write_data.byte_value);
            

        
        break; 


		default:
			break;
	}

	return 0;
}


module_init(ioctl_d_interface_init);
module_exit(ioctl_d_interface_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Akanksha");
MODULE_DESCRIPTION("IOCTL Interface driver");
MODULE_VERSION("0.1");