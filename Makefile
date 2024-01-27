obj-m += firewall.ko

# Object files needed for linkage of the module.
firewall-y := main.o hook.o logs.o rule_table.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean