#include "rule_table.h"

static int __init init()
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
}

module_init(init);
module_exit(exit);