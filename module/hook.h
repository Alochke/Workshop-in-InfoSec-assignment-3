#ifndef HOOK
#define HOOK

#include <linux/kernel.h>   /* We're doing kernel work. */
#include <linux/slab.h>     /* For kmaloc and kfree. */
#include <linux/netfilter.h>/* Next two includes are for the Netfilter API */
#include <linux/netfilter_ipv4.h>
#include "main.h"
#include "rule_table.h"
#include "logs.h"

int hook_init(void);
void hook_destroy(void);

#endif