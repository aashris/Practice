#include <linux/init.h>             // Macros used to mark up functions e.g. __init __exit
#include <linux/module.h>           // Core header for loading LKMs into the kernel
#include <linux/device.h>           // Header to support the kernel Driver Model
#include <linux/kernel.h>           // Contains types, macros, functions for the kernel
#include <linux/fs.h>               // Header for the Linux file system support
#include <linux/uaccess.h>            // Required for the copy to user function
#include <linux/list.h>
#include <linux/slab.h>

#include "readerwriter.h"

LIST_HEAD(chat_linkedlist);

typedef struct user_chat {
	char message[256];
	struct list_head list;
} chat;

void init_chat(void) {
        LIST_HEAD(chat_linkedlist);
}

int add_to_chat(char *buffer, size_t len) {
        
        chat *ptr;
        chat *input_chat = kmalloc(sizeof(*input_chat), GFP_KERNEL);
        if (input_chat == NULL) {
                printk(KERN_ERR "Unable to allocate memory for chat\n");
                return -1;
        }
        snprintf(input_chat->message, len,  buffer);
        printk(KERN_INFO "Input is %s\n", buffer);
        INIT_LIST_HEAD(&input_chat->list);

        list_add_tail(&input_chat->list, &chat_linkedlist);
        
        list_for_each_entry(ptr, &chat_linkedlist, list) {
                printk(KERN_INFO "MSG: %s", ptr->message);
        }
        return 0;
}

void read_from_chat(void) {
        chat *ptr;
        list_for_each_entry(ptr, &chat_linkedlist, list) {
                printk(KERN_INFO "MSG: %s", ptr->message);
        }
        
        printk(KERN_INFO "End of list");
}
