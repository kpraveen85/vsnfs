obj-m += vsnfsClient.o
vsnfs-objs := vsnfsClient.o vsnfsMount.o module.c

DEBUG_FLAGS = -DDEBUG

EXTRA_CFLAGS= -Wall -Werror ${DEBUG_FLAGS}

#LIBS = 

all:

    make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:

	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

