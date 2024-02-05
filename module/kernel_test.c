# include "fw.h"

static int __init fw_init(void)
{
    __u8 idk = 255;
    printk("%d\n", (long) idk);
    return 0;
}


static void __exit fw_exit(void)
{
}


module_init(fw_init);
module_exit(fw_exit);