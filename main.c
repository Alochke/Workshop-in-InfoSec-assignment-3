#include "main.h"

MODULE_SIGNATURE

int err;    /* We'll use that variable to preserve return values of failed functions.*/
int major_number;
struct class* sysfs_class = NULL;

/*
	Cleans the module according to the stage of initialization it is at.

	Parameters:
    - stg (enum stage): A designated enum's member that represents the stage of initialization the sysfs part of the module is in.
*/
void cleanup(enum stage stg)
{
	// We use the enum- stage, defined in main.h to choose action based on the state of the sysfs initialization the module is currently at. 
    switch (stg)
    {
    case FINAL:
    case CHRDEV:
        unregister_chrdev(major_number, NAME_OF_DEVICE);
	case HOOKS:
		hook_destroy();
	}
}

/*
    Module initialization function.
*/
static int __init fw_init(void)
{
    ERR_CHECK((err = hook_init()) < 0, FIRST,, err)

    // Create char device.
	ERR_CHECK((major_number = register_chrdev(0, NAME_OF_DEVICE, &fops)) < 0, HOOKS ,printk(KERN_ERR "register_chrdev failed."), major_number)

    return SUCCESS;
}

/*
    Module removal function.
*/
static void __exit LKM_exit(void)
{
}

module_init(LKM_init);
module_exit(LKM_exit);