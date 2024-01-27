obj-m += firewall.ko
firewall-objs := main.o hook.o logs.o rule_table.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean