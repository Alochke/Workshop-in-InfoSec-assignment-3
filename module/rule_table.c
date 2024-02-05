#include "rule_table.h"

rule_t* rule_table = NULL; // A pointer to the table.
unsigned char rule_table_rules_num = 0; // The number of rules currently loaded to the table.
static struct device* sysfs_device; // The sysfs device.

#define SYSFS_DEVICE "rules" // The name of the sysfs device.
#define SIZE_ERR_MSG "Rule table loading failed because you provided too much data."
#define MAX_RULES (50)
#define RULE_TABLE_DISPLAY_OFFSET 1
#define RULE_TABLE_SIZE MAX_RULES * sizeof(rule_t)
#define NO_CLEANUP_ERR_CHECK(condition, msg) MAIN_ERR_CHECK(condition,, msg)
#define NUMBR_OF_BYTES_TRANSFERED rule_table_rules_num * sizeof(rule_t)
#define DIRECTION_NUM 3 // The number of possible values for the direction member of a rule_t.
#define DIRECTION_VALS (unsigned int[DIRECTION_NUM]){DIRECTION_IN, DIRECTION_OUT, DIRECTION_ANY} // The possible values of a direction member of a rule_t.
#define ACK_NUM 3 // The number of possible values for the ack member of a rulet_t.
#define ACK_VALS (unsigned int[ACK_NUM]){ACK_YES, ACK_NO, ACK_ANY} // The possible values of an ack member of a rule_t.
#define ACTIONS_NUM 2 // The number of possible vlues for the action member of a rule_t.
#define ACTION_VALS (unsigned int[ACTIONS_NUM]){NF_ACCEPT, NF_DROP} // The possible values of the action member of a rule_t.
#define MAX_MASK_LEN 32
#define MASK_FROM_SIZE(mask_size) (~((1LU << (MAX_MASK_LEN - (mask_size))) - 1))


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

    Changes the first byte of buf to rule_num, and then writes the rule table to buf[1],
    while changing the ip and port fields to host-endianness.
    
    Returns: ((sizeof(rule_t) * rule_table_rules_num) + 1), which is the amount of bytes copied to buf.
*/
static ssize_t display(struct device *dev, struct device_attribute *attr, char *buf)
{
    size_t i; // For loop index.

    buf[0] = rule_table_rules_num;

    for (i = 0; i < rule_table_rules_num; i++)
    {
        ((rule_t*) (buf + RULE_TABLE_DISPLAY_OFFSET))[i] = rule_table[i];
        ((rule_t*) (buf + RULE_TABLE_DISPLAY_OFFSET))[i].src_ip = ntohl(rule_table[i].src_ip);
        ((rule_t*) (buf + RULE_TABLE_DISPLAY_OFFSET))[i].dst_ip = ntohl(rule_table[i].dst_ip);
        ((rule_t*) (buf + RULE_TABLE_DISPLAY_OFFSET))[i].src_port = (__be16) ntohs(rule_table[i].src_port);
        ((rule_t*) (buf + RULE_TABLE_DISPLAY_OFFSET))[i].dst_port = (__be16) ntohs(rule_table[i].dst_port);
    }

    return NUMBR_OF_BYTES_TRANSFERED + RULE_TABLE_DISPLAY_OFFSET;
}

/*
    We'll use that enum in check_correct.
    It represents the type of member we're checking in a function call.
*/
enum type{
    INT,
    CHAR
};

/*
    This function is a refactorization of the procedure of checking if a member of a rule_t is correct.

    Parameters:
        - member (unsigned int): The member whose correctness is checked.
        - values (unsigned int[]): The correct values for member.
        - len (unsigned int): The length of values.
        - t (enum type): An enum's member that represents the type of the member we're checking for correctness.

    Returns: 0 if member is correct, else, return -1.
*/
static inline int check_correct(unsigned int member, unsigned int values[], unsigned int len, enum type t)
{
    size_t i; // for loop index.
    for (i = 0; i < len; i++)
    {
        switch (t)
        {
            case INT:
                if(member == values[i])
                    return MAIN_SUCEESS;
            case CHAR:
                if((unsigned char)member == (unsigned char) values[i])
                    return MAIN_SUCEESS;
        }
    }
    return MAIN_FAILURE;
}

/*
	The implementation of store.

    Transfers count / sizeof(rule_t) rule_ts from buf to the rule_table while changing their ip, prefix_mask and port fields to network-endianness.
    Will error in case count > RULE_TABLE_SIZE.

    Returns: -1 on failure, (count / sizeof(rule_t)) on success.
*/
static ssize_t modify(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
    size_t i; // For loop index.

    // This is fine because RULE_TABLE_SIZE < PAGE_SIZE.
    NO_CLEANUP_ERR_CHECK(count > RULE_TABLE_SIZE, SIZE_ERR_MSG)

    //Checking the given values are correct.
    for(i = 0; i < count / sizeof(rule_t); i++)
    {
        if (
            check_correct(((rule_t*)buf)[i].direction, DIRECTION_VALS, DIRECTION_NUM, INT)
            || 
            check_correct(((rule_t*)buf)[i].ack, ACK_VALS, ACK_NUM, INT)
            ||
            check_correct(((rule_t*)buf)[i].action, ACTION_VALS, ACTIONS_NUM, INT)
            ||
            (((rule_t*)buf)[i].src_prefix_mask != MASK_FROM_SIZE(((rule_t*)buf)[i].src_prefix_size))
            ||
            (((rule_t*)buf)[i].dst_prefix_mask != MASK_FROM_SIZE(((rule_t*)buf)[i].dst_prefix_size))    
        )
        {
            return MAIN_FAILURE;
        }
    }
    
    for(i = 0; i < count / sizeof(rule_t); i++)
    {
        rule_table[i] = ((rule_t*)buf)[i];
        rule_table[i].src_ip = htonl(((rule_t*)buf)[i].src_ip);
        rule_table[i].src_prefix_mask = htonl(((rule_t*)buf)[i].src_prefix_mask);
        rule_table[i].dst_ip = htonl(((rule_t*)buf)[i].dst_ip);
        rule_table[i].dst_prefix_mask = htonl(((rule_t*)buf)[i].dst_prefix_mask);
        rule_table[i].src_port = (__be16) htons(((rule_t*)buf)[i].src_port);
        rule_table[i].dst_port = (__be16) htons(((rule_t*)buf)[i].dst_port);
    }
    
    rule_table_rules_num = count / sizeof(rule_t);

    return NUMBR_OF_BYTES_TRANSFERED;
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
            kfree(rule_table);
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
    MAIN_INIT_ERR_CHECK(IS_ERR(sysfs_device = device_create(sysfs_class, NULL, MKDEV(major_number, 0), NULL, SYSFS_DEVICE)), FIRST, "device_create")

    //create sysfs file attributes.
	MAIN_INIT_ERR_CHECK(device_create_file(sysfs_device, (const struct device_attribute *)&dev_attr_rules.attr), DEVICE_INIT, "device_create_file")

    MAIN_INIT_ERR_CHECK((rule_table = kmalloc(RULE_TABLE_SIZE, GFP_KERNEL)) == NULL, ATTRIBUTE_INIT, "kmalloc")

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