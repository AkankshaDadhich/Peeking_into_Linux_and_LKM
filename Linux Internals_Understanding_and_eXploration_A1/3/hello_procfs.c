#include <linux/init.h>
#include <linux/module.h>
 #include <linux/proc_fs.h>
 #include <linux/slab.h>
 #include <linux/string.h>
 //#include <asm-generic/uaccess.h>
 #include <linux/uaccess.h>
 

 MODULE_LICENSE("GPL");
 MODULE_AUTHOR("Akanksha");
 MODULE_DESCRIPTION("Simple module featuring proc read");

//My entry which will be displayed in /proc
 #define ENTRY_NAME "hello_world"
 #define PERMS 0644
 #define PARENT NULL


static char *message;
static int read_p;

//open function
int hello_proc_open(struct inode *sp_inode, struct file *sp_file) {
 printk("proc called open\n");

 read_p = 1;
 message = kmalloc(sizeof(char) * 20, GFP_KERNEL);

 if (message == NULL) {
 printk("ERROR, hello_proc_open");
 return -ENOMEM;
 }
 strcpy(message, "Hello, World!\n");
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

 printk("proc called read\n");
 copy_to_user(buf, message, len);
 return len;
 }

//release function
int hello_proc_release(struct inode *sp_inode,
struct file *sp_file) {
 printk("proc called release\n");
 kfree(message);
 return 0;
 }

// specifies which function to call while open read release
static const struct proc_ops fops = {
    .proc_open = hello_proc_open,
    .proc_read = hello_proc_read,
    .proc_release = hello_proc_release,
};

// module init function
 static int hello_init(void) {
 printk("/proc/%s create\n", ENTRY_NAME);
 
// create entry function
 if (!proc_create(ENTRY_NAME, PERMS, NULL, &fops)) {
 printk("ERROR! proc_create\n");
 remove_proc_entry(ENTRY_NAME, NULL);
 return -ENOMEM;
 }
 return 0;
 }
 module_init(hello_init);

// exit module function
 static void hello_exit(void) {
 remove_proc_entry(ENTRY_NAME,
NULL);
 printk("Removing /proc/%s.\n",
ENTRY_NAME);
 }

 module_exit(hello_exit);