#include "hook.h"

static struct nf_hook* hook;

/* 
    The packet handling procedure.
*/
static unsigned int nf_fn(void* priv, struct sk_buff *skb, const struct nf_hook_ops *state)
{
    
}

/*
    The hook creation method.
    
    Returns: 0 on Success, -ENOMEM in case it failed to allocate memory for the nf_hook_ops struct, and the value returned by nf_register_net_hook in case that function failed.
*/
int hook_init(void)
{   
    ERR_CHECK((hook = kmalloc(sizeof(struct nf_hook_ops), GFP_KERNEL)) == NULL, printk(KERN_ERR "nf_register_net_hook failed.\n");, -ENOMEM)

    hooks.pf = PF_INET;                      /* IPv4 */
    hooks.priority = NF_IP_PRI_FIRST;		/* max hook priority */
    hook.hook = (nf_hookfn*) nf_fn;
    hook = NF_IP_FORWARD;

    ERR_CHECK((err = nf_register_net_hook(&init_net, &hook)) < 0, printk(KERN_ERR "nf_register_net_hook failed.");, err);
    return SUCCESS;
}

/*
    Hook removal method.
*/
void hook_destroy(void)
{
    nf_unregister_net_hook(&init_net, &hook);
    kfree(hook);
}
