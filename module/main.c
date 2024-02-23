#include "main.h"

MAIN_MODULE_SIGNATURE


#define AUTO_MINOR 0 // When passed to register_chrdev makes it assign the registered char device a major number automatically.

int major_number; // The major number of the char device.
struct class* sysfs_class;

static struct file_operations fops = {
	.owner = THIS_MODULE,
    .read = logs_read,
    .open = logs_open
};

/*
    The next enum is for the cleanup function in main.c. Items represent the state of the module initialization the module is currently at.
*/
enum stage{
    FIRST,
    HOOK_INIT,
    CHAR_DEV_INIT,
    SYSFS_CLASS_INIT,
    RULE_TABLE_INIT,
    LOGS_INIT
};

/*
	Cleans the module.

	Parameters:
    - stg: A designated enum's member that represents the stage of initialization the module is at.
        The enum is stage, and it's defined at main.c.
*/
static void cleanup(enum stage stg)
{
    // We use the enum- stage, defined in main.c to choose action based on the state of the sysfs initialization the module is currently at. 
    switch (stg)
    {
        case FIRST:
            break;
        case LOGS_INIT:
            logs_destroy();
        case RULE_TABLE_INIT:
            rule_table_destroy();
        case SYSFS_CLASS_INIT:
            class_destroy(sysfs_class);
        case CHAR_DEV_INIT:
            unregister_chrdev(major_number, MAJOR_DEVICE);
        case HOOK_INIT:
            hook_destroy();
    }
}

/*
    This function sets the permissions for a /dev device that is created by device_create.

    It's necessary for setting the permissions of /dev/fw_log to be as expected.
*/
static char *setmode(struct device *dev, umode_t *mode)
{
    if (!mode) // We're checking that mode is not null because it can be when the module is getting destroyed.
            return NULL;
    if (dev->devt == MKDEV(major_number, LOGS_DEV_MINOR))
            *mode = 0400;
    return NULL;
}

/*
    Module initialization function.

    Returns: 0 in case of success, else, it'll return -1.
*/
static int __init fw_init(void)
{
    MAIN_INIT_ERR_CHECK(hook_init(), FIRST, "hook_init")

    // Create char device.
	MAIN_INIT_ERR_CHECK((major_number = register_chrdev(AUTO_MINOR, MAJOR_DEVICE, &fops)) < 0, HOOK_INIT, "register_chrdev")

    MAIN_INIT_ERR_CHECK(IS_ERR(sysfs_class = class_create(THIS_MODULE, CLASS)), CHAR_DEV_INIT, "class_create")

    sysfs_class->devnode = setmode; // We set a callback that ensures that the premissions of the dev devices are set as expected.

    MAIN_INIT_ERR_CHECK(rule_table_init(), SYSFS_CLASS_INIT, "rule_table_init")

    MAIN_INIT_ERR_CHECK(logs_init(), RULE_TABLE_INIT, "logs init")

    return MAIN_SUCEESS;
}

/*
    Module removal function.
*/
static void __exit fw_exit(void)
{
    cleanup(LOGS_INIT);
}

module_init(fw_init);
module_exit(fw_exit);