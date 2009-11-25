obj-m += vsnfs.o
vsnfs-objs := vsnfsClient.o vsnfsMount.o module.o super.o

EXTRA_CFLAGS= -Wall -Werror

all: vsnfs

vsnfs:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
