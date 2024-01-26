static int LKM_init(void)
{
    printk("idk" "\n");
    return 0;
}

static void LKM_exit(void)
{
}

module_init(LKM_init);
module_exit(LKM_exit);