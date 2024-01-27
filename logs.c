#include "logs.h"

#define DEV_DEVICE "fw_log" // The name of the device the user space program will interact with thraugh its /dev interface.
#define SYSFS_DEVICE "log" // The name of the device the user space program will interact with thraugh its sysfs interface.

static struct device* dev_device = NULL;
static struct device* sysfs_device = NULL;

/*
    The next enum is for the cleanup function in logs.c. Items represent the state of the logs initialization the module is currently at.
*/
enum stage{
    FIRST,
    DEV_DEVICE_INIT,
    SYSFS_DEVICE_INIT,
    ATTRIBUTE_INIT
};

/*
	The implementation of store.
*/
static ssize_t modify(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
    return 0;
}

/*
    The open implementation of the dev/ devices of the driver.

    Returns: 0
*/
int logs_open(struct inode *_inode, struct file *_file)
{
    return MAIN_SUCEESS;
}

/*
    The read implementation of the dev/ devices of the driver.

    Returns: number of bytes read.
*/
ssize_t logs_read(struct file *filp, char *buff, size_t length, loff_t *offp)
{
    return 0;
}

// Declares the attributes for the sysfs device
static DEVICE_ATTR(reset, 0200, NULL, modify);


/*
	Cleans the logs part of the module.

	Parameters:
    - stg (stage): A designated enum's member that represents the stage of initialization the logs part of the module is at.
*/
static void cleanup(enum stage stg)
{
    // We use the enum- stage, defined in logs.c to choose action based on the state of the sysfs initialization the module is currently at. 
    switch (stg)
    {
        case FIRST:
            break;
        case ATTRIBUTE_INIT:
            device_remove_file(sysfs_device, (const struct device_attribute *)&dev_attr_reset.attr);
        case SYSFS_DEVICE_INIT:
            device_destroy(sysfs_class, MKDEV(major_number, MAIN_LOG_MINOR));
        case DEV_DEVICE_INIT:
            device_destroy(sysfs_class, MKDEV(major_number, MAIN_FW_LOG_MINOR));
    }
}



/*
    Initializes the logs part of the module.
*/
int logs_init(void)
{
    // Create /dev device.
	MAIN_ERR_CHECK(IS_ERR(dev_device = device_create(sysfs_class, NULL, MKDEV(major_number, MAIN_FW_LOG_MINOR), NULL, DEV_DEVICE)), FIRST, "device_create")

    // Create sysfs device.
    MAIN_ERR_CHECK(IS_ERR(sysfs_device = device_create(sysfs_class, NULL, MKDEV(major_number, MAIN_FW_LOG_MINOR), NULL, SYSFS_DEVICE)), DEV_DEVICE_INIT, "device_create")

    // Create sysfs file attributes.
    MAIN_ERR_CHECK(device_create_file(sysfs_device, (const struct device_attribute *)&dev_attr_reset.attr), SYSFS_DEVICE_INIT, "device_create_file")

    return MAIN_SUCEESS;
}

/*
    A wrapper function around cleanup, that serves as an abstraction layer of the cleanup process of the logs part of the module,
	In case the initialization of that part of the module is done.
*/
void logs_destroy()
{
    cleanup(ATTRIBUTE_INIT);
}