#ifndef MAIN
#define MAIN
#include <linux/kernel.h>   /* We're doing kernel work. */
#include <linux/module.h>   /* We're adding a kernel module. */
#include "hook.h"

extern unsigned int accepted; /* A counter for the accepted number of packets. */
extern unsigned int dropped;  /* A counter for the dropped number of packets. */

#define SUCCESS 0

#define MODULE_SIGNATURE                                                                \
    MODULE_LICENSE("GPL");                                                              \
    MODULE_AUTHOR("Alon Polsky");                                                       \
    MODULE_DESCRIPTION("HW3 solution for the course Workshop in Information Security.");\

#define ERR_CHECK(condition, extra_code, errno_value){  \
    if(condition){                                      \
        extra_code;                                     \
        return errno_value;                             \
    }                                                   \
}

extern int err;

#endif