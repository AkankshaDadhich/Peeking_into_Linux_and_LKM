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
#include <linux/ptrace.h>
#include <linux/sched/task.h>
#include <asm/pgtable.h>



// store the major number extracted by dev_t 
int ioctl_d_interface_major = 300;
int ioctl_d_interface_minor = 0;



typedef struct
{
	atomic_t available;
	struct semaphore sem;
	struct cdev cdev;
} ioctl_d_interface_dev;


#define DEVICE_NAME "ioctlChgParent"
char* ioctl_d_interface_name = DEVICE_NAME;


ioctl_d_interface_dev ioctl_d_interface;

#define IOCTL_CHG_PARENT _IOW('q', 5, struct ioctl_data)

int ioctl_d_interface_open(struct inode *inode, struct file *filp);
int ioctl_d_interface_release(struct inode *inode, struct file *filp);
long ioctl_d_interface_ioctl(struct file *filp, unsigned int cmd, unsigned long arg);


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

struct ioctl_data {
    pid_t parent_pid;
    pid_t process_id;
	 
};



// just like the 2_I ques initilization
static int ioctl_d_interface_dev_init(ioctl_d_interface_dev * ioctl_d_interface)
{
	int result = 0;
	memset(ioctl_d_interface, 0, sizeof(ioctl_d_interface_dev));
	atomic_set(&ioctl_d_interface->available, 1);
	sema_init(&ioctl_d_interface->sem, 1);

	return result;
}

static void ioctl_d_interface_dev_del(ioctl_d_interface_dev * ioctl_d_interface) {}

//setup function
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
// function excuted whrn insmod is called
static int ioctl_d_interface_init(void)
{
	dev_t           devno = 0;
	int             result = 0;

	
	 ioctl_d_interface_dev_init(&ioctl_d_interface);

    /* register char device using register_chrdev_region */
    result = register_chrdev_region(MKDEV(300, ioctl_d_interface_minor), 1, ioctl_d_interface_name);
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
// function executed when rmmod is called
static void ioctl_d_interface_exit(void)
{
	dev_t devno = MKDEV(ioctl_d_interface_major, ioctl_d_interface_minor);

	cdev_del(&ioctl_d_interface.cdev);
	unregister_chrdev_region(devno, 1);
	ioctl_d_interface_dev_del(&ioctl_d_interface);

	printk(KERN_INFO "ioctl_d_interface: module unloaded\n");
}

//open API
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

// main IOctl function where everything happens
long ioctl_d_interface_ioctl(struct file *cd, unsigned int cmd, unsigned long arg)
{   struct ioctl_data data;
	//struct task_struct __rcu    *parent;
	switch (cmd) {
        case IOCTL_CHG_PARENT:
			
			printk(KERN_INFO "<%s> ioctl: IOCTL_CHG_PARENT\n", DEVICE_NAME);
            if (copy_from_user(&data, (struct ioctl_data *)arg, sizeof(struct ioctl_data))) {
                return -1;
            }

            // Find the task_struct of the new parent process
			struct task_struct *new_parent_task = pid_task(find_get_pid(data.parent_pid), PIDTYPE_PID);
            if (!data.parent_pid)
                return -1; 
          // Log information before the change
			printk(KERN_ALERT "Before change: Current process (PID %d) parent PID: %d\n",
      		 task_pid_nr(current), task_pid_nr(current->parent));


         current-> parent = new_parent_task;

		// new_parent_task ->children->next = current;


			list_add_tail(&current->sibling, &new_parent_task->children);

			printk(KERN_ALERT "After change: Current process (PID %d) new parent PID: %d\n",
       		task_pid_nr(current), task_pid_nr(current->parent));
            break;

        default:
            return -1;
			break;
	}

	return 0;
}


module_init(ioctl_d_interface_init);
module_exit(ioctl_d_interface_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Akanksha");
MODULE_DESCRIPTION("IOCTL Parent change Interface driver");
MODULE_VERSION("0.1");