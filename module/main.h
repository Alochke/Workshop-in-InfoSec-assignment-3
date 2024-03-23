#ifndef MAIN
#define MAIN

#include "hook.h"
#include "logs.h"
#include "rule_table.h"
#include "connection_table.h"

#define MAIN_SUCEESS 0 // What functions will retrun in case of success.
#define MAIN_FAILURE -1 // What functions will retrun in case of failure.
#define MAJOR_DEVICE "fw" // The name of the registered char device, all sysfs and dev devices will be minor instances of this char device.
#define CLASS "fw" // The name of the sysfs class, all sysfs devices related to the module will be instances of this class.

#define MAIN_MODULE_SIGNATURE                                                           \
    MODULE_LICENSE("GPL");                                                              \
    MODULE_AUTHOR("Alon Polsky");                                                       \
    MODULE_DESCRIPTION("HW3 solution for the course Workshop in Information Security.");\


#define MAIN_ERR_CHECK(condition, extra_code, msg){\
    if(unlikely(condition)){                       \
        extra_code                                 \
        printk(KERN_ERR msg "\n");                 \
        return MAIN_FAILURE;                       \
    }                                              \
}

#define MAIN_INIT_ERR_CHECK(condition, state, function_name) MAIN_ERR_CHECK(condition, cleanup(state);, function_name " has failed")

#define MAIN_SIMPLE_ERR_CHECK(condition, msg) MAIN_ERR_CHECK(condition,, msg)

extern int major_number;
extern struct class* sysfs_class;


#endif