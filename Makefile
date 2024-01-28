obj-m += firewall.o

# Object files needed for linkage of the module.
#firewall-objs := hook.o logs.o rule_table.o main.o
firewall-obj := kernel_test.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean