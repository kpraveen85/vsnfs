EXTRA_CFLAGS += -Wall -Werror -DVSNFS_DEBUG

obj-m += vsnfsd.o

vsnfsd-y := vsnfsproc.o vsnfssvc.o vsnfsxdr.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

