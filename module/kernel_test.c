# include "fw.h"

static int __init fw_init(void)
{
    __u8 idk = 256
    printk("%d\n", (__be32) idk);
    return 0;
}


static void __exit fw_exit(void)
{
}


module_init(fw_init);
module_exit(fw_exit);