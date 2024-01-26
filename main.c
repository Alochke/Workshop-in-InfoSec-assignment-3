#include "main.h"

MODULE_SIGNATURE

int err;    /* We'll use that variable to preserve return values of failed functions.*/

/*
    Module initialization function.
*/
static int __init fw_init(void)
{
    ERR_CHECK((err = hook_init) < 0,,err)
}

/*
    Module removal function.
*/
static void __exit LKM_exit(void)
{
    hook_destroy();
}

module_init(LKM_init);
module_exit(LKM_exit);