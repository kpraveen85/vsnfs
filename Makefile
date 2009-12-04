obj-m += vsnfs.o
vsnfs-y := vsnfsXdr.o vsnfsProc.o vsnfsClient.o vsnfsMount.o module.o
vsnfs-objs := $(nfs-y)

EXTRA_CFLAGS= -Wall -Werror

all: vsnfs

vsnfs:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules EXTRA_CFLAGS="$(EXTRA_CFLAGS)"

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
