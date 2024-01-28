# include "fw.h"

static int __init fw_init(void)
{
    printk("%d\n", sizeof(rule_t));
    return 0;
}


static void __exit fw_exit(void)
{
}


module_init(fw_init);
module_exit(fw_exit);