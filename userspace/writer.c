#include <stdio.h>
#include<stdlib.h>
#include<errno.h>
#include<fcntl.h>
#include<string.h>
#include<unistd.h>

//#define BUF_LENGTH 256
//static char receive[BUF_LENGTH];

void writing_to_chat() {
	int fd, ret;
	char buffer[256];

	fd = open("/dev/chatmodule", O_WRONLY);
	if (fd < 0) {
		perror("Failed to open the device...");
		//return errno;
	}
	printf ("Begin writing \n");
	for (int i = 0; i < 20; i++) {
		sprintf(buffer, "writing ... %d\n", i);
		ret = write(fd, buffer, sizeof (buffer));
		sleep(1);
	}

	printf("Writing had ended.\n");
}

int main() {
   int ret, fd;
   printf("Starting writing numbers\n");
   writing_to_chat();
   
   return 0;	
}
