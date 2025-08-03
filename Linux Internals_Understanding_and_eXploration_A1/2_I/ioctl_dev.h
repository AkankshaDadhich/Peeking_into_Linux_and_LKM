#ifndef __IOCTL_DEV_DEFINE_H__
#define __IOCTL_DEV_DEFINE_H__


// structure fo ioctl device interface it contains cdev
typedef struct
{
	atomic_t available;
	struct semaphore sem;
	struct cdev cdev;
} ioctl_d_interface_dev;


struct write_data {
    unsigned long phys_addr;
    uint8_t byte_value;
};
// functions declared here which are to used when the user calls open device
int ioctl_d_interface_open(struct inode *inode, struct file *filp);
int ioctl_d_interface_release(struct inode *inode, struct file *filp);
long ioctl_d_interface_ioctl(struct file *filp, unsigned int cmd, unsigned long arg);

#endif