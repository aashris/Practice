chatapp-objs := chatapp_module.o chatapp_rwhelper.o
obj-m += chatapp.o
all:
	make -C /lib/modules/$(shell uname -r)/build/ M=$(PWD) modules
clean:
	make -C /lib/modules/$(shell uname -r)/build/ M=$(PWD) clean
