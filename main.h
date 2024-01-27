#ifndef MAIN
#define MAIN
#include <linux/kernel.h>   /* We're doing kernel work. */
#include <linux/module.h>   /* We're adding a kernel module. */
#include <linux/fs.h> /* This and the next header are for sysfs kernel api to work. */
#include <linux/device.h> 
#include "hook.h"
#include "logs.h"
#include "rule_table.h"

#define MAIN_SUCEESS 0 // What functions will retrun in case of success.
#define MAIN_FAILURE -1 // What functions will retrun in case of failure.

#define MAIN_MODULE_SIGNATURE                                                           \
    MODULE_LICENSE("GPL");                                                              \
    MODULE_AUTHOR("Alon Polsky");                                                       \
    MODULE_DESCRIPTION("HW3 solution for the course Workshop in Information Security.");\

#define MAIN_ERR_CHECK(condition, state, function_name){\
    if(condition){                                      \
        cleanup(state)                                  \
        printk(KERN_ERR function_name " has failed\n"); \
        return MAIN_FAILURE;                            \
    }                                                   \
}

extern int major_number;
extern struct class* sysfs_class;

enum main_minors{
    MAIN_RULE_TABLE_MINOR = 0,
    MAIN_FW_LOG_MINOR = 1,
    MAIN_LOG_MINOR = 2
};

#endif