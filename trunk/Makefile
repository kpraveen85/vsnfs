obj-m += vsnfs.o
vsnfs-y := module.o vsnfsClient.o vsnfsMount.o vsnfsXdr.o vsnfsProc.o
vsnfs-objs := $(nfs-y)

EXTRA_CFLAGS= -Wall -Werror -DVSNFS_DEBUG

all: vsnfs

vsnfs:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules EXTRA_CFLAGS="$(EXTRA_CFLAGS)"

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
