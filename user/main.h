#ifndef MAIN
#define MAIN

#define <stdio.h>
#define <string.h>
#include "rule_table_parser.h"
#include "list.h"

#define MAIN_ERR_CHECK(condition, extra_code, value_returned){  \
    if(condition){                                              \
        extra_code                                              \
        return value_returned;                                  \
    }                                                           \
}

#define MAIN_SIMPLE_ERR_CHECK(condition) MAIN_ERR_CHECK(condition,,EXIT_FAILURE)

#endif