#include "logs.h"
#include "list.h"

#define DEV_DEVICE "fw_log" // The name of the device the user space program will interact with thraugh its /dev interface.
#define SYSFS_DEVICE "log" // The name of the device the user space program will interact with thraugh its sysfs interface.
#define SIZE_ERR_MSG "Were not transferred, the logs. Because too small, is the buffer you've provided."
#define WRONG_ADDRESS_ERR_MSG "Failed is the logs transferring, because of illegal addresses is the buffer you've provided."
#define ONE_COUNTED 1

#define RVAL_UINT_TO_POINTER(x) (unsigned int[1]){x}
#define RVAL_UCHAR_TO_POINTER(x) (unsigned short[1]){x}

static struct device* dev_device = NULL;
static struct device* sysfs_device = NULL;
static struct klist* log_list;
static struct klist_iter* iter;
static unsigned int row_num = 0; // The number of rows in the logs.


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
int logs_update(unsigned char protocol, unsigned char action, __be32 src_ip, __be32 dst_ip, __be16 src_port, __be16 dst_port, reason_t reason)
{
    log_node* node; // If a new row has to be added to the logs, we'll use that pointer to point to it.
    log_row_t* log_row; // Will point to the log_row_t the function points to at a given point in time.
    struct timeval ktv; // This will be used to get the current time.
    do_gettimeofday(&ktv);
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
            log_row->count += ONE_COUNTED;
            log_row->timestamp = ktv.tv_sec;
            klist_iter_exit(iter);
            return MAIN_SUCEESS;
        }
    }
    klist_iter_exit(iter);
    MAIN_ERR_CHECK((node = kmalloc(sizeof(log_node), GFP_KERNEL)) == NULL,, "kmalloc has failed");
    klist_add_tail(&node->node, log_list);
    MAIN_ERR_CHECK(node->log == NULL, klist_del(&node->node);, "kmalloc has failed"); // Checks if the get function of log_list has failed to allocate a log_row_t for the log member of node to point to and handles properly.
    log_row = (log_row_t*)node->log;
    log_row->timestamp = ktv.tv_sec;
    log_row->protocol = protocol;
    log_row->action = action;
    log_row->src_ip = src_ip;
    log_row->dst_ip = dst_ip;
    log_row->src_port = src_port;
    log_row->dst_port = dst_port;
    log_row->reason = reason;
    log_row->count = ONE_COUNTED;
    row_num += ONE_COUNTED;
    return MAIN_SUCEESS;
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
	The implementation of store for the sysfs device.

    Resets the logs.

    Returns 0
*/
static ssize_t modify(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
    list_destroy(log_list, iter);
    row_num = 0;
    return MAIN_SUCEESS;
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
    The read implementation of the dev/ device of the driver.

    If length == sizeof(unsigned int) then we write row_num to it,
    else,
    The function checks if length < sizeof(log_row_t) * row_num, where length the size of the given buffer in bytes,
    and if that's the case then it prints "Were not transferred, the logs. Because too small, is the buffer you've provided." to the kernel logs that can be seen by the dmesg shell command
    and returns -1.
    If length != sizeof(unsigned int) and (length >= sizeof(log_row_t) * row_num) then the function goes thraugh the log_list linked-list and for every node it copies its log_row_t to the buffer.
    
    Of course the function writes every byte to the user space cautiously, by using the copy_to_user function that will transfer the byte only if the destined address is not in kernel space and is not NULL,
    If one of the addresses didn't follow this, then "Failed is the logs transferring, because of illegal addresses is the buffer you've provided." will be written to the to the kernel logs that can be seen by the dmesg shell command
    and the function returns -1.

    Thus, partial writes are possible on error.

    Returns: Number of bytes written into the buffer on success or in case a partial write has accrued, and -1 on error.
*/
ssize_t logs_read(struct file *filp, char *buff, size_t length, loff_t *offp)
{
    size_t num_copied = 0; // Number of bytes copied to the buffer.
    size_t copy_curr = 0; // Number of bytes copied in a given iteration of going thraugh the log_list list.
    if (length == sizeof(unsigned int))
    {
        num_copied = sizeof(unsigned int) - copy_to_user(buff, &row_num, sizeof(unsigned int));
        if (unlikely(num_copied < sizeof(unsigned int)))
        {
            num_copied = MAIN_FAILURE;
            printk(KERN_ERR WRONG_ADDRESS_ERR_MSG "\n");
        }
    }
    else
    {
        MAIN_SIMPLE_ERR_CHECK(length < sizeof(log_row_t) * row_num, SIZE_ERR_MSG);
        for (klist_iter_init(log_list, iter); klist_next(iter) != NULL;)
        {
            copy_curr = sizeof(log_row_t) - copy_to_user(buff + num_copied, node_to_log(iter->i_cur), sizeof(log_row_t));
            copy_to_user(buff + num_copied + offsetof(log_row_t, src_ip), RVAL_UINT_TO_POINTER(ntohl(node_to_log(iter->i_cur)->src_ip)), sizeof(__be32));
            copy_to_user(buff + num_copied + offsetof(log_row_t, dst_ip), RVAL_UINT_TO_POINTER(ntohl(node_to_log(iter->i_cur)->dst_ip)), sizeof(__be32));
            copy_to_user(buff + num_copied + offsetof(log_row_t, src_port), RVAL_UCHAR_TO_POINTER(ntohs(node_to_log(iter->i_cur)->src_port)), sizeof(unsigned short));
            copy_to_user(buff + num_copied + offsetof(log_row_t, dst_port), RVAL_UCHAR_TO_POINTER(ntohs(node_to_log(iter->i_cur)->dst_port)), sizeof(unsigned short));
            num_copied += copy_curr;
            if (unlikely(copy_curr < sizeof(log_row_t)))
            {
                num_copied = MAIN_FAILURE;
                printk(KERN_ERR WRONG_ADDRESS_ERR_MSG "\n");
                break;
            }
        }
        klist_iter_exit(iter);
    }
    return num_copied;     
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
            device_destroy(sysfs_class, MKDEV(major_number, LOGS_SYSFS_MINOR));
        case DEV_DEVICE_INIT:
            device_destroy(sysfs_class, MKDEV(major_number, LOGS_DEV_MINOR));
    }
}



/*
    Initializes the logs part of the module.
*/
int logs_init(void)
{
    // Create /dev device.
	MAIN_INIT_ERR_CHECK(IS_ERR(dev_device = device_create(sysfs_class, NULL, MKDEV(major_number, LOGS_DEV_MINOR), NULL, DEV_DEVICE)), FIRST, "device_create")

    // Create sysfs device.
    MAIN_INIT_ERR_CHECK(IS_ERR(sysfs_device = device_create(sysfs_class, NULL, MKDEV(major_number, LOGS_SYSFS_MINOR), NULL, SYSFS_DEVICE)), DEV_DEVICE_INIT, "device_create")

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