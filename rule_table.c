#include "rule_table.h"

static rule_t* rule_tables = NULL; // A pointer to the table.
static int rule_num = 0; // The number of rules currently loaded to the table.
static struct device* sysfs_device; // The sysfs device.

#define SYSFS_DEVICE "rules" // The name of the sysfs device.

/*
    The next enum is for the cleanup function in rule_table.c. Items represent the state of the rule_table initialization the module is currently at.
*/
enum stage{
    FIRST,
    DEVICE_INIT,
    ATTRIBUTE_INIT
};

/*
	The implementation of show.
*/
static ssize_t display(struct device *dev, struct device_attribute *attr, char *buf)
{
    return MAIN_SUCEESS;
}

/*
	The implementation of store.
*/
static ssize_t modify(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
    return MAIN_SUCEESS;
}

// Declares the attributes for the device.
static DEVICE_ATTR(rules, 0600, display, modify);

/*
	Cleans the rule_table part of the module.

	Parameters:
    - stg (stage): A designated enum's member that represents the stage of initialization the rule_table part of the module is at.
*/
static void cleanup(enum stage stg)
{
	// We use the enum- stage, defined in rule_table.c to choose action based on the state of the sysfs initialization the module is currently at. 
	switch (stg)
	{
        case FIRST:
            break;
        case ATTRIBUTE_INIT:
            device_remove_file(sysfs_device, (const struct device_attribute *)&dev_attr_rules.attr);
	    case DEVICE_INIT:
		    device_destroy(sysfs_class, MKDEV(major_number, MAIN_RULE_TABLE_MINOR));
	}
}

/*
    Initiates the rule table.

    Returns: 0 in case succeeded, else, it'll return -1.
*/
int rule_table_init()
{
    //create sysfs device.
    MAIN_ERR_CHECK(IS_ERR(sysfs_device = device_create(sysfs_class, NULL, MKDEV(major_number, MAIN_RULE_TABLE_MINOR), NULL, SYSFS_DEVICE)), FIRST, "device_create")

    //create sysfs file attributes.
	MAIN_ERR_CHECK(device_create_file(sysfs_device, (const struct device_attribute *)&dev_attr_rules.attr), DEVICE_INIT, "device_create_file")

    return MAIN_SUCEESS;
}

/*
    A wrapper function of cleanup, that serves as an abstraction layer of the cleanup process of the rule_table part of the module,
	In case the initialization of that part of the module is done.
*/
void rule_table_destroy()
{
    cleanup(ATTRIBUTE_INIT);
}