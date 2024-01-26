#ifndef MAIN
#define MAIN
#include <linux/kernel.h>   /* We're doing kernel work. */
#include <linux/module.h>   /* We're adding a kernel module. */
#include "hook.h"
#include "logs.h"

#define SUCCESS 0
#define NAME_OF_DEVICE "fw"
#define NAME_OF_CLASS "fw"

extern int err;
extern class* sysfs_class;

#define MODULE_SIGNATURE                                                                \
    MODULE_LICENSE("GPL");                                                              \
    MODULE_AUTHOR("Alon Polsky");                                                       \
    MODULE_DESCRIPTION("HW3 solution for the course Workshop in Information Security.");\

#define ERR_CHECK(condition, state, extra_code, errno_value){   \
    if(condition){                                              \
        cleanup(state);                                         \
        extra_code;                                             \
        return errno_value;                                     \
    }                                                           \
}

static struct file_operations fops = {
	.owner = THIS_MODULE
};

/*
    The next enum is for the clean_up function in main.c. Items represent the state of the module initialization the module is currently at.
*/
enum stage{
    FIRST = 0,
    HOOKS = 1,
    CHRDEV = 2,
    CLASS =3
};

#endif