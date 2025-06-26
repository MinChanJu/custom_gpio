obj-m += custom_gpio.o

KDIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)

all:
	$(MAKE) -C $(KDIR) M=$(PWD) modules
	$(MAKE) -C app
	sudo insmod custom_gpio.ko
	@echo "[+] custom_gpio.ko inserted"

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean
	$(MAKE) -C app clean
	sudo rmmod custom_gpio
	@echo "[-] custom_gpio.ko removed"