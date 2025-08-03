#include <linux/init.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/slab.h>
#include <linux/string.h>
//#include <asm-generic/uaccess.h>
#include <linux/uaccess.h>
// #include <linux/vmstat.h>
#include <linux/mm.h>
#include<linux/vm_event_item.h>
#include <linux/page_counter.h>
#include<linux/kernel.h>


 

 MODULE_LICENSE("GPL");
 MODULE_AUTHOR("Akanksha");
 MODULE_DESCRIPTION("Getting Page fault");

// entry name and permission
 #define ENTRY_NAME "get_page_fault"
 #define PERMS 0644
 #define PARENT NULL


static char *message;
static int read_p;

// open function
int my_proc_open(struct inode *sp_inode, struct file *sp_file) {
 printk(KERN_INFO "proc called open\n");
 read_p = 1;
 message = kmalloc(sizeof(char) * 100, __GFP_RECLAIM | __GFP_IO | __GFP_FS);

 if (message == NULL) {
 printk (KERN_INFO "error in proc_open\n");
 return -1;
 }
// ev[PGFAULT] is used to find the page fault using all_vm_events
unsigned long ev[NR_VM_EVENT_ITEMS];
all_vm_events(ev);
sprintf (message, "Page Faults in system are- %lu\n", ev[PGFAULT]);
 return 0;
 }

// read fucntion
ssize_t hello_proc_read(struct file *sp_file, char __user *buf, size_t
size, loff_t *offset) {


 int len = strlen(message);

 read_p = !read_p;
 if (read_p) {
 return 0;
 }

printk(KERN_INFO "Read procfs\n");
 copy_to_user(buf, message, len);

 return len;
 }







int hello_proc_release(struct inode *sp_inode,
struct file *sp_file) {
 
 printk (KERN_INFO "proc called release\n");
 kfree(message);
 return 0;
 }


static struct proc_ops fops = {
    .proc_open = my_proc_open,
    .proc_read = hello_proc_read,
    .proc_release = hello_proc_release,
};

 static int hello_init(void) {
 printk(KERN_INFO "/proc/%s create\n", ENTRY_NAME);
 if (!proc_create(ENTRY_NAME, PERMS, NULL, &fops)) {
 printk("ERROR! proc_create\n");
 remove_proc_entry(ENTRY_NAME, NULL);
 return -ENOMEM;
 }
 return 0;
 }

static void hello_exit(void) {
 remove_proc_entry(ENTRY_NAME,
NULL);
 printk("Removing /proc/%s.\n",
ENTRY_NAME);
 }
 
 module_init(hello_init);
 module_exit(hello_exit);