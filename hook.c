#include "hook.h"

/* 
    The next part is kind of disgusting, sorry.
    I wrote it only because I found out that the only way to use this MACROS is to undef __kernel__ so we can get the macros from linux/netfilter_ipv4.h...
    But, if we'll do that the compiler will inevitably try to import limits.h, which is undefined, I suppose because we are compiling kernel-space code.
    Anyway, because I felt like that's the start of falling down a rabbit hole, I deccided to go with this less beutiful soulution of simply defining the macro.
*/
#define NF_IP_FORWARD   2 /* If the packet is destined for another interface. */

static struct nf_hook* hook;

/*
    The next enum is for the cleanup function in hook.c. Items represent the state of the hook initialization the module is currently at.
*/
static enum stage
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
        case HOOK_INIT:
            nf_unregister_net_hook(&init_net, &hook);
        case ALLOCATION:
            kfree(hook);
    }
}

/* 
    The packet handling procedure.
*/
static unsigned int nf_fn(void* priv, struct sk_buff *skb, const struct nf_hook_ops *state)
{
    
}

/*
    The hook creation method.
    
    Returns: 0 on MAIN_SUCEESS and -1 in case of failure.
*/
int hook_init(void)
{   
    MAIN_ERR_CHECK((hook = kmalloc(sizeof(struct nf_hook_ops), GFP_KERNEL)) == NULL, FIRST, "kmalloc")

    hooks.pf = PF_INET;                     /* IPv4 */
    hooks.priority = NF_IP_PRI_FIRST;		/* max hook priority */
    hook.hook = (nf_hookfn*) nf_fn;
    hook = NF_IP_FORWARD;

    MAIN_ERR_CHECK(nf_register_net_hook(&init_net, &hook) < 0, ALLOCATION, "nf_register_net_hook");
    return MAIN_SUCEESS;
}

/*
    A wrapper function around cleanup, that serves as an abstraction layer of the cleanup process of the hook part of the module,
	In case the initialization of that part of the module is done.
*/
void hook_destroy(void)
{
    cleanup(HOOK_INIT);
}
