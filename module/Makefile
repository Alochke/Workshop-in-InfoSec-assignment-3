obj-m += firewall.o

# Object files needed for linkage of the module.
firewall-objs := hook.o logs.o rule_table.o hw4secws.o list.o connection_table.o
COMPILATION-FILES := firewall.o *.mod.* *.symvers *.order

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
	rm $(firewall-objs) $(COMPILATION-FILES)

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean