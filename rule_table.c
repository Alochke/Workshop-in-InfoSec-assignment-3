#include "rule_table.h"

static rule_t* rule_table = NULL; // A pointer to the table.
static int rules_num = 0; // The number of rules currently loaded to the table.
static struct device* sysfs_device; // The sysfs device.

#define SYSFS_DEVICE "rules" // The name of the sysfs device.
#define DATA_FORMAT_MSG "Rule table loading failed because you provided too much data or data of the wrong format."
#define FAILED_KMALLOC_MSG "Rule table memory allocation failed."
#define ZERO_BYTES 0 // The number of bytes modify copied from the supplied buffer on failure.
#define NO_CLEANUP_ERR_CHECK(condition, msg) MAIN_ERR_CHECK(condition,, msg)

/*
    The next enum is for the cleanup function in rule_table.c. Items represent the state of the rule_table initialization the module is currently at.
*/
enum stage{
    FIRST,
    DEVICE_INIT,
    ATTRIBUTE_INIT,
    RULE_TABLE_ALLOC
};

/*
	The implementation of show.

    Copies rules_num * sizeof(rule_t) from rule_table to buf.
    
    Returns: sizeof(rule_t) * rules_num, which is the amount of bytes copied from the user space program to buf.
*/
static ssize_t display(struct device *dev, struct device_attribute *attr, char *buf)
{
    for (size_t i = 0; i < rules_num; i++)
    {
        ((rule_t*)buf)[i] = rule_table[i];
    }
    
    return sizeof(rule_t) * rules_num;
}

/*
	The implementation of store.

    Trasnfers count bytes as they are from buf to an allocated kernel buffer pointed by rule_table and frees the buffer that wass pointed by rule_table previously,
    if there was such.
    Will not do any of this if count / sizeof(rule_t) > FW_MAX_RULES (meaning, too much data was provided.) or count % sizeof(rule_t) != 0 (meaning, the data was not sizeof(rule_t) aligned.),
    which counts as an error.

    Returns: -1 on failure, count on success.
*/
static ssize_t modify(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
    size_t temp = count / sizeof(rule_t); // This will store the rule_num, till modify will end, then its value will be passed to rules_num.

    NO_CLEANUP_ERR_CHECK((temp > FW_MAX_RULES) || (count % sizeof(rule_t) != 0), DATA_FORMAT_MSG)
    
    MAIN_INIT_ERR_CHECK((rule_table = kmalloc(count, GFP_KERNEL)) == NULL, FAILED_KMALLOC_MSG)

    if (rule_table != NULL)
    {
        kfree(rule_table);
    }

    for (size_t i = 0; i < temp; i++)
    {
        rule_table[i] = *((rule_t*) (buf + sizeof(rule_t) * i));
    }

    rules_num = temp;

    return count;
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
        case RULE_TABLE_ALLOC:
            if (rule_table != NULL)
            {
                kfree(RULE_TABLE);
            }
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
    MAIN_INIT_ERR_CHECK(IS_ERR(sysfs_device = device_create(sysfs_class, NULL, MKDEV(major_number, MAIN_RULE_TABLE_MINOR), NULL, SYSFS_DEVICE)), FIRST, "device_create")

    //create sysfs file attributes.
	MAIN_INIT_ERR_CHECK(device_create_file(sysfs_device, (const struct device_attribute *)&dev_attr_rules.attr), DEVICE_INIT, "device_create_file")

    return MAIN_SUCEESS;
}

/*
    A wrapper function of cleanup, that serves as an abstraction layer of the cleanup process of the rule_table part of the module,
	In case the initialization of that part of the module is done.
*/
void rule_table_destroy()
{
    cleanup(RULE_TABLE_ALLOC);
}