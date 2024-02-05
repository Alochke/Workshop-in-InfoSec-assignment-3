#include "hook.h"

#define IN_STR "enp0s8"
#define OUT_STR "enp0s9"
#define SRTCMP_OF_EQ 0
#define CHECK_DIRECTION(VAL, STR) ((rule_table[i].direction & VAL) && (SRTCMP_OF_EQ == strcmp(interface, STR))) // Checks if the direction of a packet is in the domain of enforcement of a rule, used in nf_fn.
#define CHECK_IP(rule_ip, packet_ip, mask) ((rule_ip & mask) == (packet_ip & mask)) // Checks if packet_ip is a member of the subnet defined by rule_ip/mask.
#define CHECK_PORT(rule_port, packet_port) ((rule_port == FW_PORT_ANY) || ((rule_port == FW_PORT_ABOVE_1023) && (packet_port > 1023)) || (rule_port == packet_port))
/* 
    The next part is kind of disgusting, sorry.
    I wrote it only because I found out that the only way to use this MACROS is to undef __kernel__ so we can get the macros from linux/netfilter_ipv4.h...
    But, if we'll do that the compiler will inevitably try to import limits.h, which is undefined, I suppose because we are compiling kernel-space code.
    Anyway, because I felt like that's the start of falling down a rabbit hole, I deccided to go with this less beutiful soulution of simply defining the macro.
*/
#define NF_IP_FORWARD   2 /* If the packet is destined for another interface. */

static struct nf_hook_ops *hook;

/*
    The next enum is for the cleanup function in hook.c. Items represent the state of the hook initialization the module is currently at.
*/
enum stage
{
    FIRST,
    ALLOCATION,
    HOOK_INIT
};


/*
	Cleans the hook part of the module.

	Parameters:
    - stg (stage): A designated enum's member that represents the stage of initialization the hook part of the module is at.
*/
static void cleanup(enum stage stg)
{
    // We use the enum- stage, defined in hook.c to choose action based on the state of the hook initialization the module is currently at. 
    switch (stg)
    {
        case FIRST:
            break;
        case HOOK_INIT:
            nf_unregister_net_hook(&init_net, hook);
        case ALLOCATION:
            kfree(hook);
    }
}

/*
    Checks for coruption of the rule_t member- member.

    Parameters:
    - member (long): The member of the rule_t we check for corruption.
    - values (long): The list of possible values for member.
    - len (unsigned int): The length of values.
*/
int check_coruption(long member, long values[], unsigned int len)
{
    for (size_t i = 0; i < len; i++)
    {
        if (member == values[i])
        {
            return MAIN_SUCEESS;
        }
    }
    return MAIN_FAILURE;
}

/* 
    The packet handling procedure.
*/
static unsigned int nf_fn(void* priv, struct sk_buff *skb, const struct nf_hook_ops *state)
{
    /*
        First we check if the skb is empty, and if so, we let this packet continue on its routing. While this seems a bit unnecessary, I added this
        because I have seen it in the article I added a link to below. I need skb to be unempty for the checks to work so this won't damage anything.
        https://infosecwriteups.com/linux-kernel-communication-part-1-netfilter-hooks-15c07a5a5c4e
    */
    if (!skb)
    {
        return NF_ACCEPT;
    }
    
    char* interface = skb->dev->name;
    __be32 src_ip = ip_hdr(skb)->saddr;
    __be32 dst_ip = ip_hdr(skb)->daddr;
    __u8 protocol = ip_hdr(skb)->protocol;
    __be16 src_port, dst_port;
    ack_t ack;

    if ((protocol != PROT_TCP) && (protocol != PROT_UDP) && (protocol != PROT_ICMP))
    {
        rturn NF_ACCEPT;
    }

    if (protocol == PROT_TCP)
    {
        src_port = tcp_hdr(skb)->source;
        dst_port = tcp_hdr(skb)->dest;
        ack = (ack_t) tcp_hdr(skb)->ack;
    }
    else if (protocol == PROT_UDP)
    {
        src_port = udp_hdr(skb)->source;
        dst_port = udp_hdr(skb)->dest;
    }

    for(int i = 0; i < rule_table_rules_num; i++)
    {
        if (
            (CHECK_DIRECTION(DIRECTION_IN, IN_STR) || CHECK_DIRECTION(DIRECTION_OUT, OUT_STR))
            &&
            CHECK_IP(rule_table[i].src_ip, src_ip, rule_table[i].src_prefix_mask)
            &&
            CHECK_IP(rule_table[i].dst_ip, dst_ip, rule_table[i].dst_prefix_mask)
            &&
            (
                (protocol == PROT_ICMP)
                ||
                (
                    CHECK_PORT(rule_table[i].src_port, src_port)
                    &&
                    CHECK_PORT(rule_table[i].dst_port, dst_port)
                )
            )
            &&
            ((rule_table[i].protocol == PROT_ANY) || (rule_table[i].protocol == protocol))
            &&
            ((protocol != PROT_TCP) || ((rule_table[i].ack & ACK_NO) && !ack) || ((rule_table[i].ack & ACK_YES) && ack))
        )  
        {
            return rule_table[i].action;
        }
    }

    return NF_DROP;
}

/*
    The hook creation method.
    
    Returns: 0 on MAIN_SUCEESS and -1 in case of failure.
*/
int hook_init(void)
{
    MAIN_INIT_ERR_CHECK((hook = kmalloc(sizeof(struct nf_hook_ops), GFP_KERNEL)) == NULL, FIRST, "kmalloc")

    (*hook).pf = PF_INET;                     /* IPv4 */
    (*hook).priority = NF_IP_PRI_FIRST;		/* max hook priority */
    (*hook).hook = (nf_hookfn*) nf_fn;
    (*hook).hooknum = NF_IP_FORWARD;

    MAIN_INIT_ERR_CHECK(nf_register_net_hook(&init_net, hook) < 0, ALLOCATION, "nf_register_net_hook");
    return MAIN_SUCEESS;
}

/*
    A wrapper function around cleanup, that serves as an abstraction layer of the cleanup process of the hook part of the module,
	In case the initialization of that part of the module is done.
*/
void hook_destroy(void)
{
    cleanup(ALLOCATION);
}
