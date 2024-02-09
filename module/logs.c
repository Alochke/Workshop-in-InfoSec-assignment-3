#include "logs.h"
#include "list.h"

#define DEV_DEVICE "fw_log" // The name of the device the user space program will interact with thraugh its /dev interface.
#define SYSFS_DEVICE "log" // The name of the device the user space program will interact with thraugh its sysfs interface.
#define VOID_ERR_CHECK(condition, extra_code, function) {   \
    if(condition)                                           \
    {                                                       \
        printk(KERN_ERR function " has failed\n");          \
        return;                                             \
    }                                                       \
}
#define ONE_PACKET_COUNTED 1

static struct device* dev_device = NULL;
static struct device* sysfs_device = NULL;
static struct klist* log_list;
static struct klist_iter* iter;


/*
    Logs packet with the given parameters in the logs.

    Parameters:
    - protocol: The transport protocol number of the packet.
    - action: The action the hook has taken about the paket, will be one of NF_ACCEPT, NF_DROP.
    - src_ip: The source ip of the packet.
    - dst_ip: The destination ip of the packet.
    - src_port: The source port of the packet, will be zero for every packet which is not TCP/UDP.
    - dst_port: The destination port of the packet, will be zero for every packet which is not TCP/UDP.
    - reason: The reason for action.
*/
void logs_update(unsigned char protocol, unsigned char action, __be32 src_ip, __be32 dst_ip, __be16 src_port, __be16 dst_port, reason_t reason)
{
    log_node* node;
    log_row_t* log_row;
    for (klist_iter_init(log_list, iter); klist_next(iter) != NULL;)
    {
        log_row = node_to_log(iter->i_cur);
        if (
            log_row->protocol == protocol
            &&
            log_row->action == action
            &&
            log_row->src_ip == src_ip
            &&
            log_row->dst_ip == dst_ip
            &&
            log_row->src_port == src_port
            &&
            log_row->dst_port == dst_port
            &&
            log_row->reason ==  reason
        )
        {
            log_row->count += ONE_PACKET_COUNTED;
            log_row->timestamp = ktime_get_resolution_ns();
            klist_iter_exit(iter);
            return;
        }
    }
    klist_iter_exit(iter);
    VOID_ERR_CHECK((node = kmalloc(sizeof(struct log_node), GFP_KERNEL)) == NULL,, "kmalloc");
    klist_add_tail(&node->node, log_list);
    VOID_ERR_CHECK(node->log == NULL, klist_del(node->node),); // Checking if the get function of log_list has failed to allocate a log_row_t for the log member of node to point to and handling properly.
    log_row = (log_row_t*)node->log;
    log_row->timestamp = ktime_get_resolution_ns();
    log_row->protocol = protocol;
    log_row->action = action;
    log_row->src_ip = src_ip;
    log_row->dst_ip = dst_ip;
    log_row->src_port = src_port;
    log_row->dst_port = dst_port;
    log_row->reason = reason;
    log_row->count = ONE_PACKET_COUNTED;
}

/*
    The next enum is for the cleanup function in logs.c. Items represent the state of the logs initialization the module is currently at.
*/
enum stage{
    FIRST,
    DEV_DEVICE_INIT,
    SYSFS_DEVICE_INIT,
    ATTRIBUTE_INIT,
    ITER_INIT,
    LIST_INIT
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
        case LIST_INIT:
            list_destroy(log_list, iter);
            kfree(log_list);
        case ITER_INIT:
            kfree(iter);
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
	MAIN_INIT_ERR_CHECK(IS_ERR(dev_device = device_create(sysfs_class, NULL, MKDEV(major_number, MAIN_FW_LOG_MINOR), NULL, DEV_DEVICE)), FIRST, "device_create")

    // Create sysfs device.
    MAIN_INIT_ERR_CHECK(IS_ERR(sysfs_device = device_create(sysfs_class, NULL, MKDEV(major_number, MAIN_LOG_MINOR), NULL, SYSFS_DEVICE)), DEV_DEVICE_INIT, "device_create")

    // Create sysfs file attributes.
    MAIN_INIT_ERR_CHECK(device_create_file(sysfs_device, (const struct device_attribute *)&dev_attr_reset.attr), SYSFS_DEVICE_INIT, "device_create_file")

    MAIN_INIT_ERR_CHECK((iter = kmalloc(sizeof(struct klist_iter), GFP_KERNEL)) == NULL, ATTRIBUTE_INIT, "kmalloc")

    MAIN_INIT_ERR_CHECK((log_list = kmalloc(sizeof(struct klist) ,GFP_KERNEL)) == NULL, ITER_INIT, "kmalloc")

    klist_init(log_list, list_get, list_put);

    return MAIN_SUCEESS;
}

/*
    A wrapper function around cleanup, that serves as an abstraction layer of the cleanup process of the logs part of the module,
	In case the initialization of that part of the module is done.
*/
void logs_destroy()
{
    cleanup(LIST_INIT);
}