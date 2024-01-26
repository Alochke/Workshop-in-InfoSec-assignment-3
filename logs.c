#include "logs.h"

/*
    Initializes the logs part of the module.
*/
int logs_init(void)
{
    //create sysfs device
	ERR_CHECK(IS_ERR(sysfs_device = device_create(sysfs_class, NULL, MKDEV(major_number, 0), NULL, NAME_OF_CLASS "_" NAME_OF_DEVICE)), cleanup(SECOND); printk(KERN_ERR "device_create failed"), (int) sysfs_device)
}