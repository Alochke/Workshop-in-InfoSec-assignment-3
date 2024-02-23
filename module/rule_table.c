#include "rule_table.h"

rule_t* rule_table = NULL; // A pointer to the table.
unsigned char rule_table_rules_num = 0; // The number of rules currently loaded to the table.
static struct device* sysfs_device; // The sysfs device.

#define SYSFS_DEVICE "rules" // The name of the sysfs device.
#define SIZE_ERR_MSG "Failed is the rule table loading, because too much data you provided."
#define FORMAT_ERR_MSG "Failed is the rule table loading, because data of the wrong format, you provided."
#define MAX_RULES (50)
#define RULE_TABLE_DISPLAY_OFFSET 1 // This is to legitimize the reason we're shifting the seralized data by 1, we've to do so because we use the first byte to transfer rule_table_rules_num.
#define RULE_TABLE_SIZE (MAX_RULES * sizeof(rule_t))
#define NUMBR_OF_BYTES_TRANSFERED rule_table_rules_num * sizeof(rule_t)
#define DIRECTION_NUM 3 // The number of possible values for the direction member of a rule_t.
#define DIRECTION_VALS (unsigned int[DIRECTION_NUM]){DIRECTION_IN, DIRECTION_OUT, DIRECTION_ANY} // The possible values of a direction member of a rule_t.
#define ACK_NUM 3 // The number of possible values for the ack member of a rulet_t.
#define ACK_VALS (unsigned int[ACK_NUM]){ACK_YES, ACK_NO, ACK_ANY} // The possible values of an ack member of a rule_t.
#define ACTIONS_NUM 2 // The number of possible vlues for the action member of a rule_t.
#define ACTION_VALS (unsigned int[ACTIONS_NUM]){NF_ACCEPT, NF_DROP} // The possible values of the action member of a rule_t.
#define PROTS_NUM 4 // The number of possible values for the prot member of a rulet_t.
#define PROT_VALS (unsigned int[PROTS_NUM]){PROT_TCP, PROT_UDP, PROT_ICMP, PROT_ANY} // The legal values of a protocol member of a rule_t.
#define MAX_MASK_LEN 32
#define MASK_FROM_SIZE(mask_size) ((mask_size == 0) ? 0 : ~((1LU << (MAX_MASK_LEN - (mask_size))) - 1))

/*
	The implementation of sysfs's show.

    Changes the first byte of buf to rule_num, and then writes the rule table to &buf[1],
    while changing the IP and port fields to host-endianness.
    
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
    This function is a refactorization of the procedure of checking if a member of a rule_t is correct.
    This is used when loading a rule to the rule table.

    Parameters:
        - member: The member whose correctness is checked.
        - values: The correct values for member.
        - len: The length of values.

    Returns: 0 if member is correct, else, return -1.
*/
static inline int check_correct(unsigned int member, unsigned int values[], unsigned int len)
{
    size_t i; // for loop index.
    for (i = 0; i < len; i++)
    {
        if(member == values[i])
            return MAIN_SUCEESS;
    }
    return MAIN_FAILURE;
}

/*
	The implementation of store.

    Transfers count / sizeof(rule_t) rule_ts from buf to the rule_table while changing their IP, prefix_mask, and port fields to network-endianness.
    Will error in case count > RULE_TABLE_SIZE, or the given buffer has undefined values.

    Returns: -1 on failure, (count / sizeof(rule_t)) on success.
*/
static ssize_t modify(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
    size_t i; // For loop index.

    // This is sufficient input size checking because RULE_TABLE_SIZE < PAGE_SIZE.
    MAIN_SIMPLE_ERR_CHECK(count > RULE_TABLE_SIZE, SIZE_ERR_MSG)

    //Checking the given values are correct.
    for(i = 0; i < count / sizeof(rule_t); i++)
    {
        MAIN_SIMPLE_ERR_CHECK(
            check_correct(((rule_t*)buf)[i].direction, DIRECTION_VALS, DIRECTION_NUM)
            || 
            check_correct(((rule_t*)buf)[i].ack, ACK_VALS, ACK_NUM)
            ||
            check_correct(((rule_t*)buf)[i].action, ACTION_VALS, ACTIONS_NUM)
            ||
            check_correct(((rule_t*)buf)[i].protocol, PROT_VALS, PROTS_NUM)
            ||
            (((rule_t*)buf)[i].src_prefix_mask != MASK_FROM_SIZE(((rule_t*)buf)[i].src_prefix_size))
            ||
            (((rule_t*)buf)[i].dst_prefix_mask != MASK_FROM_SIZE(((rule_t*)buf)[i].dst_prefix_size)),
            FORMAT_ERR_MSG
        )
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
    The next enum is for the cleanup function in rule_table.c. Items represent the state of the rule_table initialization the module is currently at.
*/
enum stage{
    FIRST,
    DEVICE_INIT,
    ATTRIBUTE_INIT,
    RULE_TABLE_ALLOC
};

/*
	Cleans the rule_table part of the module.

	Parameters:
    - stg: A designated enum member that represents the stage of initialization the rule_table part of the module is at.
        This is enum is defined in rule_table.c.
*/
static void cleanup(enum stage stg)
{
	// We use the enum- stage, defined in rule_table.c to choose action based on the state of the rule table initialization the module is currently at. 
	switch (stg)
	{
        case FIRST:
            break;
        case RULE_TABLE_ALLOC:
            kfree(rule_table);
        case ATTRIBUTE_INIT:
            device_remove_file(sysfs_device, (const struct device_attribute *)&dev_attr_rules.attr);
	    case DEVICE_INIT:
		    device_destroy(sysfs_class, MKDEV(major_number, RULE_TABLE_MINOR));
	}
}

/*
    Initiates the rule table.

    Returns: 0 in case of success, else, it'll return -1.
*/
int rule_table_init()
{
    //create sysfs device.
    MAIN_INIT_ERR_CHECK(IS_ERR(sysfs_device = device_create(sysfs_class, NULL, MKDEV(major_number, RULE_TABLE_MINOR), NULL, SYSFS_DEVICE)), FIRST, "device_create")

    //create sysfs file attributes.
	MAIN_INIT_ERR_CHECK(device_create_file(sysfs_device, (const struct device_attribute *)&dev_attr_rules.attr), DEVICE_INIT, "device_create_file")

    MAIN_INIT_ERR_CHECK((rule_table = kmalloc(RULE_TABLE_SIZE, GFP_KERNEL)) == NULL, ATTRIBUTE_INIT, "kmalloc")

    return MAIN_SUCEESS;
}

/*
    A wrapper function around cleanup, that serves as an abstraction layer of the cleanup process of the rule_table part of the module,
	in case the initialization of that part of the module is done.
*/
void rule_table_destroy()
{
    cleanup(RULE_TABLE_ALLOC);
}