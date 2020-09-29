/* kernel module for chat application */
#include <linux/init.h>             // Macros used to mark up functions e.g. __init __exit
#include <linux/module.h>           // Core header for loading LKMs into the kernel
#include <linux/device.h>           // Header to support the kernel Driver Model
#include <linux/kernel.h>           // Contains types, macros, functions for the kernel
#include <linux/fs.h>               // Header for the Linux file system support
#include <linux/uaccess.h>            // Required for the copy to user function

#include "readerwriter.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Aashri Shridhar Kandya");
MODULE_DESCRIPTION("A simple example chat application");
MODULE_VERSION("0.01");

#define DEVICE_NAME "chatmodule"
#define CLASS_NAME "ashnik"

static int major_number;
static char msg[256];
static int size_of_message;
static int numofusers;
static struct class*  ashnikClass  = NULL; ///< The device-driver class struct pointer
static struct device* ashnikDevice = NULL; ///< The device-driver device struct pointer

static int dev_open(struct inode *, struct file *);
static int dev_release(struct inode *, struct file *);
static ssize_t dev_read(struct file *, char *, size_t, loff_t *);
static ssize_t dev_write(struct file *, const char *, size_t, loff_t *);

static struct file_operations fops =
{
     .open = dev_open,
     .read = dev_read,
     .write = dev_write,
     .release = dev_release,
};

static int chatapp_init(void) {
      printk("Registering ashnik by Aashri");
      major_number = register_chrdev(0, DEVICE_NAME, &fops);
      if (major_number<0){
          printk(KERN_ALERT "Ashnik failed to register a major number\n");
          return major_number;
      }
      printk(KERN_INFO "Ashnik: registered correctly with major number %d\n", major_number);

      // Register the device class
      ashnikClass = class_create(THIS_MODULE, CLASS_NAME);
      if (IS_ERR(ashnikClass)){                // Check for error and clean up if there is
          unregister_chrdev(major_number, DEVICE_NAME);
          printk(KERN_ALERT "Failed to register device class\n");
          return PTR_ERR(ashnikClass);          // Correct way to return an error on a pointer
      }
      printk(KERN_INFO "Ashnik: device class registered correctly\n");
     
      // Register the device driver
      ashnikDevice = device_create(ashnikClass, NULL, MKDEV(major_number, 0), NULL, DEVICE_NAME);
      if (IS_ERR(ashnikDevice)){               // Clean up if there is an error
      class_destroy(ashnikClass);           // Repeated code but the alternative is goto statements
            unregister_chrdev(major_number, DEVICE_NAME);
            printk(KERN_ALERT "Failed to create the device\n");
            return PTR_ERR(ashnikDevice);
      }
      printk(KERN_INFO "Ashnik: device driver registered correctly\n");
      return 0;
}

// exit module
static void chatapp_exit(void){
     device_destroy(ashnikClass, MKDEV(major_number, 0));     // remove the device
     class_unregister(ashnikClass);                          // unregister the device class
     class_destroy(ashnikClass);                             // remove the device class
     unregister_chrdev(major_number, DEVICE_NAME);             // unregister the major number
     printk(KERN_INFO "Ashnik chatapp: Goodbye from the LKM!\n");
}

static int dev_open(struct inode *inodep, struct file *filep){
     numofusers++;
     printk(KERN_INFO "Ashnik chatapp: User %d  has been added to the chat\n", numofusers);
     return 0;
}

static int dev_release(struct inode *inodep, struct file *filep){
     printk(KERN_INFO "Ashnik chatapp: Closing chat\n");
     numofusers--;
     return 0;
}

static ssize_t dev_read(struct file *filep, char *buffer, size_t len, loff_t *offset){

        int error_count = 0;

        read_from_chat();
     
        // copy_to_user has the format ( * to, *from, size) and returns 0 on success
        error_count = copy_to_user(buffer, msg, size_of_message);

        if (error_count==0){            // if true then have success
                printk(KERN_INFO "Ashnik: Sent %d characters to the user\n", size_of_message);
                return (size_of_message=0);  // clear the position to the start and return 0
        }
        else {
                printk(KERN_INFO "Ashnik: Failed to send %d characters to the user\n", error_count);
                return -EFAULT;              // Failed -- return a bad address message (i.e. -14)
        }
      
        return 0;
}

static ssize_t dev_write(struct file *filep, const char *buffer, size_t len, loff_t *offset){
      
        //char* user_buf;
        if (copy_from_user(msg, buffer, len)) {
                printk(KERN_ERR "Unable to copy from user");
                return -1;       
        } 
        add_to_chat(msg, len);
        //sprintf(msg, "%s(%zu letters)", buffer, len);   // appending received string with its length
        //size_of_message = strlen(msg);                 // store the length of the stored message
        printk(KERN_INFO "Ashnik: Received %zu characters from the user\n", len);
        return len;
}

module_init(chatapp_init);
module_exit(chatapp_exit);
