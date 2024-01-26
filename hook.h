#ifndef HOOK
#define HOOK

#include <linux/kernel.h>   /* We're doing kernel work. */
#include <linux/slab.h>     /* For kmaloc and kfree. */
#include <linux/errno.h>    /* For standard error numbering. */
#include <linux/netfilter.h>/* Next two includes are for the Netfilter API */
#include <linux/netfilter_ipv4.h>
#include "main.h"


/* 
    The next part is kind of disgusting, sorry.
    I wrote it only because I found out that the only way to use this MACROS is to undef __kernel__ so we can get the macros from linux/netfilter_ipv4.h...
    But, if we'll do that the compiler will inevitably try to import limits.h, which is undefined, I suppose because we are compiling kernel-space code.
    Anyway, because I felt like that's the start of falling down a rabbit hole, I deccided to go with this less beutiful soulution of simply defining the macros.
*/
#define NF_IP_FORWARD           2 /* If the packet is destined for another interface. */

int hook_init(void);
void hook_destroy(void);

#endif