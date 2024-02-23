============================= main.c =============================
    
    "And one c file to rule them all..."

    Here the initialization and clean-up of the module happens.
    Setting an environment for the other c files to initialize, by registering a character device and a sysfs class and calling the initialization functions of the other c files.
    This c file is also responsible for calling the cleanup function of the other modules in the right order.
    This is the c file that ties all other c files together.

    static void cleanup(enum stage stg)
        Cleans the module.

        Parameters:
        - stg: A designated enum's member that represents the stage of initialization the module is at. The enum is stage, and it's defined at main.c.


    static char *setmode(struct device *dev, umode_t *mode)
        This function sets the permissions for a /dev device that is created by device_create.

        It's necessary for setting the permissions of /dev/fw_log to be as expected.

    static int __init fw_init(void)
        Module initialization function.

        Returns: 0 in case of success, else, it'll return -1.

    static void __exit fw_exit(void)
        Module removal function.

============================= hook.c =============================
    
    This c file implements the hook, the hook is responsible for inspecting the packets that are passed to it, comparing their header fields to the rule table's row fields, which are supplied by the static row table that is given by rule_table.c, the hook is also reponsible for calling the functions that are provided by log.c with the appropriate paramters for logging the packets.

    static void cleanup(enum stage stg)
    	Cleans the hook part of the module.

        Parameters:
        - stg: A designated enum's member that represents the stage of initialization the hook part of the module is at.

    static unsigned int nf_fn(void* priv, struct sk_buff *skb, const struct nf_hook_state *state)
        The packet handling procedure.

        Checks if a packet is tcp or udp or icmp, other types of transport protocols are accepted,
        The tcp, udp and icmp packets are checked against a match for every rules domain from the top of the loaded rule table to the bottom.
        When a match is found the hook accepts or drops the packet, according to the rule's verdict,
        If no match was found, a tcp/udp/icmp packet is dropped.

        In any case, as long as the provided sk_buff is not null, the hooks actions, the reason for them and the packet's data is logged.

        Returns: 1 on acceptance, 0 when dropping.

    int hook_init(void)
        The hook creation function.
        
        Returns: 0 on success and -1 in case of failure.

    void hook_destroy(void)
        A wrapper function around cleanup, that serves as an abstraction layer of the cleanup process of the hook part of the module,
        In case the initialization of that part of the module is done.

============================= list.c =============================
    
    This file implements a linked list that is used by logs.c,
    It utilizes the kernel klist API, implemented and documented at include/linux/klist.h and /lib/klist.c:
    https://elixir.bootlin.com/linux/v4.15/source/lib/klist.c
    https://elixir.bootlin.com/linux/v4.15/source/include/linux/klist.h#L20
    Each node includes a log entery and an embeded klist_node struct, that is used by the klist API.

    void list_put(struct klist_node* node)
        The put function of the klist, will be useful for log_node-related memory deallocation.
        This function is called each time a node is removed from the list, and in our use-case every time klist_del is called on a node, with the node as the function's parameter.

        Parameters:
            node: The node that is being removed from a list.

    void list_get(struct klist_node* node)
        The get function of the klist, will be useful for log_node-related memory allocation.
        This function is called each time a node is added to the list, and in our use-case every time klist_add_tail is called on a node, with the node as the function's parameter.

        Parameters:
            node: The node that is being added to to the list.

    void list_destroy(struct klist *list, struct klist_iter *iter)
        Resets list, by using iter.
        This doesn't free list, for reusabilty reasons.
            
        Parameters:
            - list: A pointer to the list we're destroying, it will be log_list from list.c in practice.
            - iter: A pointer to the iter we'll use to iterate over list, it will be klist_iter from list.c in parctice.


============================= logs.c =============================

    This c file implements the logs, it's respobsible for creating the char devices used for logging (the /dev and the sysfs device), their attributes and to clean them and the logs linked-list data structure.
    It is also responsible for logging packets. It's function that does so, logs_update is called by the hook function, nf_fn in hook.c, and gets the logged data from the calling funtion.
    This c file is also implements the input and output functions of the devices associated with the logs, so it clears the logs when the function modify is called by writing to /sys/class/fw/log/reset from the userspace and transfers the seralized logs to the userspace by reading from /dev/fw_log from the userspace (see the documantation of modify and logs_read to understand how to use those functions.)

    int logs_update(unsigned char protocol, unsigned char action, __be32 src_ip, __be32 dst_ip, __be16 src_port, __be16 dst_port, reason_t reason)
        Logs packet with the given parameters in the logs.

        Parameters:
        - protocol: The transport protocol number of the packet.
        - action: The action the hook had taken about the packet, will be one of NF_ACCEPT, NF_DROP.
        - src_ip: The source ip of the packet.
        - dst_ip: The destination ip of the packet.
        - src_port: The source port of the packet, will be zero for every packet which is not TCP/UDP.
        - dst_port: The destination port of the packet, will be zero for every packet which is not TCP/UDP.
        - reason: The reason for action.

    static ssize_t modify(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
        The implementation of store for the sysfs device.
        Resets the logs.

        Returns 0


