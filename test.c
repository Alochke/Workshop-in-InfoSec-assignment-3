#include <linux/kernel.h>
#include <linux/module.h>

static int __init LKM_init(void)
{
    printk("idk" "\n");
    return 0;
}

static void __exit LKM_exit(void)
{
}

module_init(LKM_init);
module_exit(LKM_exit);