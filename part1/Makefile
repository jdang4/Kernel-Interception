obj-m := part1.o
KDIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)

all:
	$(MAKE) -C $(KDIR) M=$(PWD) modules
	gcc testOpenClose.c -o testOpenClose

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean
	rm -f testOpenClose