#include "connection_table.h"

#define DEVICE "conns" // The name of the device the user space program will interact with thraugh its /sysfs interface.

static struct device* device = NULL; // The device for reading the connection table.
static struct klist* connection_table; // The connection table.
static struct klist_iter* iter; // An klist_iter we'll use to iterate over the connection_table.

// Declares the attributes for the device
static DEVICE_ATTR(conns, 0400, NULL, NULL);

/*
    The next enum is for the cleanup function in connection_table.c. Items represent the state of the logs initialization the module is currently at.
*/
enum stage{
    FIRST,
    DEVICE_INIT,
    ATTRIBUTE_INIT,
    ITER_INIT,
    LIST_INIT
};

/*
	Cleans the logs part of the module.

	Parameters:
    - stg: A designated enum member that represents the stage of initialization the connection part of the module is at. This enum is defined in logs.c
*/
static void cleanup(enum stage stg)
{
    // We use the enum- stage, defined in connection_table.c to choose action based on the stage of the sysfs initialization the module is currently at. 
    switch (stg)
    {
        case FIRST:
            break;
        case LIST_INIT:
            list_destroy(connection_table, iter);
            kfree(connection_table);
        case ITER_INIT:
            kfree(iter);
        case ATTRIBUTE_INIT:
            device_remove_file(device, (const struct device_attribute *)&dev_attr_conns.attr);
        case DEVICE_INIT:
            device_destroy(sysfs_class, MKDEV(major_number, CONNECTION_TABLE_MINOR));
    }
}



/*
    Initializes the connection table part of the module.
*/
int connection_table_init(void)
{
    // Create device.
	MAIN_INIT_ERR_CHECK(IS_ERR(device = device_create(sysfs_class, NULL, MKDEV(major_number, CONNECTION_TABLE_MINOR), NULL, DEVICE)), FIRST, "device_create")

    // Create sysfs attributes.
    MAIN_INIT_ERR_CHECK(device_create_file(device, (const struct device_attribute *)&dev_attr_conns.attr), DEVICE_INIT, "device_create_file")

    MAIN_INIT_ERR_CHECK((iter = kmalloc(sizeof(struct klist_iter), GFP_KERNEL)) == NULL, ATTRIBUTE_INIT, "kmalloc")

    MAIN_INIT_ERR_CHECK((connection_table = kmalloc(sizeof(struct klist) ,GFP_KERNEL)) == NULL, ITER_INIT, "kmalloc")

    klist_init(connection_table, NULL, NULL);

    return MAIN_SUCEESS;
}

/*
    A wrapper function around cleanup, that serves as an abstraction layer of the cleanup process of the connection_table part of the module,
	in case the initialization of that part of the module is done.
*/
void connection_table_destroy()
{
    cleanup(LIST_INIT);
}