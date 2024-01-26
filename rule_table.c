#include "rule_table.h"

static int __init LKM_init(void)
{
    printk("%d\n", sizeof(sizeof(rule_t)) * MAX_RULES);
    printk("%lu\n", PAGE_SIZE);
    return 0;
}

/*
    Module removal function.
*/
static void __exit LKM_exit(void)
{
}

module_init(LKM_init);
module_exit(LKM_exit);