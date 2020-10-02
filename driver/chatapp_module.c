// SPDX-License-Identifier: GPL-2.0
/* kernel module for chat application */
#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/uaccess.h>

#include "readerwriter.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Aashri Shridhar Kandya");
MODULE_DESCRIPTION("A simple example chat application");
MODULE_VERSION("0.01");

#define DEVICE_NAME "chatmodule"
#define CLASS_NAME "chatapp"

static int major_number;
static char msg[256];
static int size_of_message;
static int numofusers;
static struct class *chatappClass;
static struct device *chatappDevice;

static int chatapp_dev_open(struct inode *, struct file *);
static int chatapp_dev_release(struct inode *, struct file *);
static ssize_t chatapp_dev_read(struct file *, char *, size_t, loff_t *);
static ssize_t chatapp_dev_write(struct file *, const char *, size_t, loff_t *);

static const struct file_operations fops = {
	.open = chatapp_dev_open,
	.read = chatapp_dev_read,
	.write = chatapp_dev_write,
	.release = chatapp_dev_release,
};

static int chatapp_init(void)
{
	dev_info("Registering chatapp by Aashri");
	major_number = register_chrdev(0, DEVICE_NAME, &fops);
	if (major_number < 0) {
		dev_alert("Chatapp failed to register a major number\n");
		return major_number;
	}
	dev_info("Chatapp: registered correctly with major# %d\n",
		major_number);

	// Register the device class
	chatappClass = class_create(THIS_MODULE, CLASS_NAME);
	if (IS_ERR(chatappClass)) {
		unregister_chrdev(major_number, DEVICE_NAME);
		dev_alert("Failed to register device class\n");
		return PTR_ERR(chatappClass);
	}
	dev_info("Chatapp: device class registered correctly\n");

	// Register the device driver
	chatappDevice = device_create(chatappClass, NULL,
				MKDEV(major_number, 0), NULL, DEVICE_NAME);
	if (IS_ERR(chatappDevice)) {
		class_destroy(chatappClass);
		unregister_chrdev(major_number, DEVICE_NAME);
		dev_alert("Failed to create the device\n");
		return PTR_ERR(chatappDevice);
	}
	dev_info("Chatapp: device driver registered correctly\n");
	return 0;
}

// exit module
static void chatapp_exit(void)
{
	device_destroy(chatappClass, MKDEV(major_number, 0));
	class_unregister(chatappClass);
	class_destroy(chatappClass);
	unregister_chrdev(major_number, DEVICE_NAME);
	dev_info("Chatapp: Goodbye from the LKM!\n");
}

static int chatapp_dev_open(struct inode *inodep, struct file *filep)
{
	numofusers++;
	dev_info("Chatapp: User %d added to the chat\n", numofusers);
	return 0;
}

static int chatapp_dev_release(struct inode *inodep, struct file *filep)
{
	dev_info("Chatapp: Closing chat\n");
	numofusers--;
	return 0;
}

static ssize_t chatapp_dev_read(struct file *filep,
				char *buffer, size_t len, loff_t *offset)
{

	int error_count = 0;

	read_from_chat();
	error_count = copy_to_user(buffer, msg, size_of_message);

	if (error_count == 0) {            // if true then have success
		dev_info("Chatapp: Sent %d characters to the user\n",
			size_of_message);
		return (size_of_message = 0);
	}

	dev_alert("Chatapp: Failed to send %d chars\n", error_count);
	return -EFAULT;
}

static ssize_t chatapp_dev_write(struct file *filep, const char *buffer,
					size_t len, loff_t *offset)
{
	if (copy_from_user(msg, buffer, len)) {
		dev_alert("Unable to copy from user");
		return -1;
	}
	add_to_chat(msg, len);
	dev_info("Chatapp: Received %zu characters from the user\n",
		len);
	return len;
}

module_init(chatapp_init);
module_exit(chatapp_exit);
