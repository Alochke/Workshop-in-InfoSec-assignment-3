# include "fw.h"

static int __init fw_init(void)
{
    printk(sizeof(rule_t));
}


static void __exit fw_exit(void)
{
}


module_init(fw_init);
module_exit(fw_exit);