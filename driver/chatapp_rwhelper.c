// SPDX-License-Identifier: GPL-2.0
#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/list.h>
#include <linux/slab.h>

#include "readerwriter.h"

LIST_HEAD(chat_linkedlist);

struct chat {
	char message[256];
	struct list_head list;
};

void init_chat(void)
{
	LIST_HEAD(chat_linkedlist);
}

int add_to_chat(char *buffer, size_t len)
{
	struct chat *ptr;
	struct chat *input_chat = kmalloc(sizeof(*input_chat), GFP_KERNEL);

	if (input_chat == NULL)
		return -1;

	snprintf(input_chat->message, len,  buffer);
	dev_info("Input is %s\n", buffer);
	INIT_LIST_HEAD(&input_chat->list);

	list_add_tail(&input_chat->list, &chat_linkedlist);

	list_for_each_entry(ptr, &chat_linkedlist, list) {
		dev_info("MSG: %s", ptr->message);
	}
	return 0;
}

void read_from_chat(void)
{
	struct chat *ptr;

	list_for_each_entry(ptr, &chat_linkedlist, list) {
		dev_info("MSG: %s", ptr->message);
	}

	dev_info("End of list");
}
