#include "rule_table.h"

int __init init()
{
    printk("%d\n", sizeof(sizeof(rule_t) * MAX_RULES));
    printk("%lu\n", PAGE_SIZE);
    return 0;
}

/*
    Module removal function.
*/
static void __exit exit(void)
{
    hook_destroy();
}

module_init(LKM_init);
module_exit(LKM_exit);