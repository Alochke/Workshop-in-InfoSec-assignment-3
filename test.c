#include <linux/kernel.h>
#include <linux/module.h>
#include "fw.h"

static int __init LKM_init(void)
{
    printk("%d" "\n", sizeof(rule_t * FW_MAX_RULES));
    printk("%d", PAGE_SIZE);
    return 0;
}

static void __exit LKM_exit(void)
{
}

module_init(LKM_init);
module_exit(LKM_exit);