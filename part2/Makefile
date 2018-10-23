obj-m := part2.o
KDIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)

all:
	$(MAKE) -C $(KDIR) M=$(PWD) modules
	gcc testCalls.c -o testCalls
	gcc testNull.c -o testNull
	gcc testNoList.c -o testNoList
clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean
	rm -f testCalls
	rm -f testNull
	rm -f testNoList
