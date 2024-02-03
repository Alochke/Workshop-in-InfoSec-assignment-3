#ifndef MAIN
#define MAIN

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "list.h"
#include "rule_table_parser.h"

#define MAIN_ARG_ERR_MSG "Wrong arguments, you've entered.\nThe correct arguments are:\n•show_rules\n•load_rules <path_to_rules_file>\n•show_log\n•clear_log\n"
#define MAIN_FILE_OPEN_ERR_MSG "Wrong file path, you've given. Or permissions for it, you lack.\n"
#define MAIN_MALLOC_ERR_MSG "Failed, is the allocation.\n"
#define MAIN_FILE_FORMAT_ERR_MSG "Wrong format, the file you've given is in.\n"
#define MAIN_RULE_TABLE_OPENNING_ATTRIBUTE_ERR_MSG "Failed openning /sys/class/fw/rules/rules.\n"
#define MAIN_RULE_TABLE_WRITING_ATTRIBUTE_ERR_MSG "Writing to ./sys/class/fw/rules/rules, the program failed.\n"

#define MAIN_ERR_CHECK(condition, extra_code){  \
    if(condition){                              \
        extra_code                              \
        return EXIT_FAILURE;                    \
    }                                           \
}

#endif