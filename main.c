#include "main.h"

MAIN_MODULE_SIGNATURE

#define MAJOR_DEVICE "fw" // The name of the registered char device, all sysfs and dev devices will be minor instances of this char device.
#define CLASS "fw" // The name of the sysfs class, all sysfs devices related to the module will be instances of this class.

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
    - stg (stage): A designated enum's member that represents the stage of initialization the module is at.
*/
static void cleanup(enum stage stg)
{
    // We use the enum- stage, defined in main.c to choose action based on the state of the sysfs initialization the module is currently at. 
    switch (stg)
    {
        case LOGS_INIT:
            logs_destroy();
        case RULE_TABLE_INIT:
            rule_table_destroy();
        case SYSFS_CLASS_INIT:
            class_destroy(sysfs_class);
        case CHAR_DEV_INIT:
            hook_destroy();
        case HOOK_INIT:
            hook_destroy();
        case FIRST:
    }
}

/*
    Module initialization function.

    Returns: 0 in case of success, else, it'll return -1.
*/
static int __init fw_init(void)
{
    MAIN_ERR_CHECK(hook_init(), FIRST, "hook_init")

    // Create char device.
	MAIN_ERR_CHECK((major_number = register_chrdev(0, MAJOR_DEVICE, &fops)) < 0, HOOK_INIT, "register_chrdev")

    MAIN_ERR_CHECK(IS_ERR(sysfs_class = class_create(THIS_MODULE, CLASS)), CHAR_DEV_INIT, "class_create")

    MAIN_ERR_CHECK(rule_table_init(), SYSFS_CLASS_INIT, "rule_table_init")

    MAIN_ERR_CHECK(logs_init(), RULE_TABLE_INIT, "logs init")

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