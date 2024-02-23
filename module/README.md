============================= main.c =============================
    
    "And one c file to rule them all..."

    Here the initialization and clean-up of the module happens.
    Setting an environment for the other c files to initialize, by registering a character device and a sysfs class and calling the initialization functions of the other c files.
    This c file is also responsible for calling the cleanup function of the other files in the right order.
    This is the c file that ties all other c files together.

    static void cleanup(enum stage stg)
        Cleans the module.

        Parameters:
        - stg: A designated enum member that represents the stage of initialization the module is at. The enum is stage, and it's defined in main.c.

    static char *setmode(struct device *dev, umode_t *mode)
        This function sets the permissions for a /dev device that is created by device_create.

        It's necessary for setting the permissions of /dev/fw_log to be as expected.

    static int __init fw_init(void)
        Module initialization function.

        Returns: 0 in case of success, else, it'll return -1.

    static void __exit fw_exit(void)
        Module removal function.

============================= hook.c =============================
    
    This c file implements the hook, the hook is responsible for inspecting the packets that are passed to it, comparing their header fields to the rule table's row fields, which are supplied by the static row table that is given by rule_table.c, the hook is also responsible for calling the functions that are provided by log.c with the appropriate parameters for logging the packets.

    static void cleanup(enum stage stg)
        Cleans the hook part of the module.

        Parameters:
        - stg: A designated enum member that represents the stage of initialization the hook part of the module is at. This is enum is defined in hook.c.

    static unsigned int nf_fn(void* priv, struct sk_buff *skb, const struct nf_hook_state *state)
        The packet handling procedure.

        Checks if a packet is tcp or udp or icmp, other types of transport protocols are accepted,
        The tcp, udp, and icmp packets are checked against a match for every rule's domain from the top of the loaded rule table to the bottom.
        When a match is found the hook accepts or drops the packet, according to the rule's verdict,
        If no match is found, a tcp/udp/icmp packet is dropped.

        In any case, as long as the provided sk_buff is not null, the hook's actions, the reason for them, and the packet's data are logged.

        Returns: 1 on acceptance, 0 when dropping.

    int hook_init(void)
        The hook creation function.
        
        Returns: 0 on success and -1 in case of failure.

    void hook_destroy(void)
        A wrapper function around cleanup, that serves as an abstraction layer of the cleanup process of the hook part of the module,
        in case the initialization of that part of the module is done.

============================= list.c =============================
    
    This file implements a linked list that is used by logs.c,
    It utilizes the kernel klist API, implemented and documented at include/linux/klist.h and /lib/klist.c:
    https://elixir.bootlin.com/linux/v4.15/source/lib/klist.c
    https://elixir.bootlin.com/linux/v4.15/source/include/linux/klist.h#L20
    Each node includes a log entry and an embedded klist_node struct, that is used by the klist API.

    void list_put(struct klist_node* node)
        The put function of the klist. It will be useful for log_node-related memory deallocation. (log_node is the struct that stores list-nodes obviously.)
        This function is called each time a node is removed from the list, and in our use-case every time klist_del is called on a node, with node as klist_del's parameter.

        Parameters:
            node: The node that is being removed from a list.

    void list_get(struct klist_node* node)
        The get function of the klist. It will be useful for log_node-related memory allocation. (log_node is the struct that stores list-nodes obviously.)
        This function is called each time a node is added to the list, and in our use-case every time klist_add_tail is called on a node, with node as klist_add_tail's parameter.

        Parameters:
            node: The node that is being added to a list.

    void list_destroy(struct klist *list, struct klist_iter *iter)
        Resets list, by using iter.
        This doesn't free list, for reusability reasons.
            
        Parameters:
            - list: A pointer to the list we're destroying, it will be log_list from list.c in practice.
            - iter: A pointer to the iter we'll use to iterate over the list, it will be klist_iter from list.c in practice.

============================= logs.c =============================

    This c file implements the logs, it's responsible for creating the char devices used for logging (the /dev and the sysfs device), their attributes, and cleaning them and the logs linked-list data structure.
    It is also responsible for logging packets. Its function that does so, logs_update, is called by the hook function, nf_fn, in hook.c, and gets the logged data from the calling function.
    This c file also implements the io functions of the devices associated with the logs, so it clears the logs when the function modify is called by writing to /sys/class/fw/log/reset from the userspace and transfers the serialized logs to the userspace when /dev/fw_log is read from the userspace (see the documentation of modify and logs_read to understand how to use those functions.)

    int logs_update(unsigned char protocol, unsigned char action, __be32 src_ip, __be32 dst_ip, __be16 src_port, __be16 dst_port, reason_t reason)
        Logs packet with the given parameters in the logs.

        Parameters:
        - protocol: The transport protocol number of the packet.
        - action: The action the hook had taken about the packet, will be one of NF_ACCEPT, NF_DROP.
        - src_ip: The source IP of the packet.
        - dst_ip: The destination IP of the packet.
        - src_port: The source port of the packet, will be zero for every packet which is not TCP/UDP.
        - dst_port: The destination port of the packet, will be zero for every packet which is not TCP/UDP.
        - reason: The reason for action.

        Returns: 0 on success, -1 on failure.

    static ssize_t modify(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
        The implementation of store for the sysfs device.
        Resets the logs.

        Returns: 0

    ssize_t logs_read(struct file *file, char *buff, size_t length, loff_t *offp)
        The read implementation of the /dev device of the driver.

        If length == sizeof(unsigned int) then we write row_num to it,
        else,
        The function checks if length < sizeof(log_row_t) * row_num, where length is the size of the given buffer in bytes,
        and if that's the case then it prints "Were not transferred, the logs. Because too small, is the buffer you've provided." to the kernel logs that can be seen by the dmesg shell command and returns -1.
        If length != sizeof(unsigned int) and (length >= sizeof(log_row_t) * row_num) then the function goes through the log_list linked-list and for every node it copies its log_row_t to the buffer.
        (row_num is a global variable in logs.c that stores an unsigned int that counts the number of rows in the logs and log_row_t is a struct that stores a serialization of such row.)
        
        Of course, the function writes every byte to the user space cautiously, by using the copy_to_user function that will transfer the byte only if the destined address is not in kernel space and is not NULL,
        If one of the addresses didn't follow this, then "Failed is the logs transferring, because of illegal addresses is the buffer you've provided." will be written to the to the kernel logs that can be seen by the dmesg shell command and the function returns -1.

        Thus, partial writes are possible on error.

        Returns: Number of bytes written into the buffer on success and -1 on error.

    static void cleanup(enum stage stg)
        Cleans the logs part of the module.

        Parameters:
        - stg: A designated enum member that represents the stage of initialization the logs part of the module is at. This enum is defined in logs.c

    int logs_init(void)
        Initializes the logs part of the module.

    void logs_destroy()
        A wrapper function around cleanup, that serves as an abstraction layer of the cleanup process of the logs part of the module,
        in case the initialization of that part of the module is done.

============================= rule_table.c =============================
    
    This c file implements the rule table data structure, its initialization, loading, and cleanup on module removal.

    static ssize_t display(struct device *dev, struct device_attribute *attr, char *buf)
        The implementation of sysfs's show.

        Changes the first byte of buf to rule_num, and then writes the rule table to &buf[1],
        while changing the IP and port fields to host-endianness.
        
        Returns: ((sizeof(rule_t) * rule_table_rules_num) + 1), which is the amount of bytes copied to buf.

    static inline int check_correct(unsigned int member, unsigned int values[], unsigned int len)
        This function is a refactorization of the procedure of checking if a member of a rule_t is correct.
        This is used when loading a rule to the rule table.

        Parameters:
            - member: The member whose correctness is checked.
            - values: The correct values for member.
            - len: The length of values.

        Returns: 0 if member is correct, else, return -1.

    static ssize_t modify(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
        The implementation sysfs's of store.

        Transfers count / sizeof(rule_t) rule_ts from buf to the rule_table while changing their ip, prefix_mask, and port fields to network-endianness.
        Will error in case count > (maximal size of a rule_table which is 50 * sizeof(rule_t), rule_t is defined in fw.h), or the given buffer has undefined values.

        Returns: -1 on failure, (count / sizeof(rule_t)) on success.
    
    static void cleanup(enum stage stg)
        Cleans the rule_table part of the module.

        Parameters:
        - stg: A designated enum's member that represents the stage of initialization the rule_table part of the module is at, the enum is defined in rule_table.c

    int rule_table_init()
        Initiates the rule table.

        Returns: 0 in case of success, else, it'll return -1.

    void rule_table_destroy()
        A wrapper function around cleanup, that serves as an abstraction layer of the cleanup process of the rule_table part of the module,
        in case the initialization of that part of the module is done.

note: I didn't use error codes through the code, and while it may be more professional to use them, it requires major changes and analysis so due to time-shortage, I have decided to leave it as is for now. Just wanted to let you know that I am aware of how you are supposed to write kernel code.