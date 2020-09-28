#include <stdio.h>
#include<stdlib.h>
#include<errno.h>
#include<fcntl.h>
#include<string.h>
#include<unistd.h>

#define BUF_LENGTH 256
static char receive[BUF_LENGTH];

void reading_from_chat() {
	int fd, ret;

	fd = open("/dev/chatmodule", O_RDONLY);
	if (fd < 0) {
		perror("Failed to open the device...");
		//return errno;
	}

	for (int i = 0; i < 20; i++) {
		ret = read(fd, receive, BUF_LENGTH);
		printf("MSG: %s", receive);
		sleep(1);
	}
}

int main() {
   printf("Reading numbers\n");
   reading_from_chat();
   
   return 0;	
}
