/*
 Assignment 1
 Question 1
 Avery Zeiler (zeilera, 400305001) and Clara Dawang (dawangc, 400329049)
 Due: February 15th, 2023
*/

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/jiffies.h>
#include <linux/proc_fs.h>
#include <asm/uaccess.h>

#define BUFFER_SIZE 128

#define PROC_NAME "seconds"
#define MESSAGE "Time elapsed since kernel module was loaded: %lu\n"
// Note that the elapsed time will be stored as an UNSIGNED LONG.

unsigned long START_JIFFIES;        // Records jiffies once module is loaded; needed to find elapsed jiffies

ssize_t proc_read(struct file *file, char *buf, size_t count, loff_t *pos);

// NOTE: this question follows structure of proc example in assignment 1 folder on Avenue!
static struct file_operations proc_ops = {
        .owner = THIS_MODULE,
        .read = proc_read,
};

// Called when module is loaded
// Creates /proc/seconds file and stores initial jiffies value (used to calculate elapsed time)
int proc_init(void)
{
    START_JIFFIES = jiffies;
    proc_create(PROC_NAME, 0, NULL, &proc_ops);
    // Should print when "dmesg" command is entered in terminal
    printk(KERN_INFO "/proc/%s created\n", PROC_NAME);
	return 0;
}

// Called when module is removed
// Removes /proc/seconds file
void proc_exit(void) {
    remove_proc_entry(PROC_NAME, NULL);
    // Should print when "dmesg" command is entered in terminal
    printk(KERN_INFO "/proc/%s removed\n", PROC_NAME);
}

// Called whenever /proc/seconds is read (ie use command "cat /proc/seconds")
// Calculates time elapsed since kernel module was loaded and displays that value
ssize_t proc_read(struct file *file, char __user *usr_buf, size_t count, loff_t *pos)
{
    int err = 0;
    char buffer[BUFFER_SIZE];
    static int completed = 0;
    // Time elapsed since kernel module is loaded = (current jiffies - start jiffies)/frequency
    unsigned long time_elapsed = (jiffies - START_JIFFIES)/HZ;

    if (completed) {
        completed = 0;
        return 0;
    }

    completed = 1;

    err = sprintf(buffer, "Time elapsed since kernel module was loaded: %lu \n", time_elapsed);

    // Copies buffer contents to user space; ie displays the elapsed time in terminal
    if (copy_to_user(usr_buf, buffer, err)) {
        printk(KERN_ERR "Error copying data to user space\n");
        return -EFAULT;
    }

    return err;
}

// Defines module entry and exit points
module_init( proc_init );
module_exit( proc_exit );

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Seconds Module");
MODULE_AUTHOR("Avery Zeiler and Clara Dawang");

