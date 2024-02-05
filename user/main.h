#ifndef MAIN
#define MAIN

#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "list.h"
#include "rule_table.h"

#define MAIN_ARG_ERR_MSG "Wrong arguments, you've entered.\nThe correct arguments are:\n•show_rules\n•load_rules <path_to_rules_file>\n•show_log\n•clear_log"
#define MAIN_FILE_OPEN_ERR_MSG "Wrong file path, you've given. Or permissions for it, you lack."
#define MAIN_MALLOC_ERR_MSG "Failed, the program an allocation."
#define MAIN_FILE_FORMAT_ERR_MSG "Wrong format, the file you've given is in."
#define MAIN_RULE_TABLE_OPENNING_ATTRIBUTE_ERR_MSG "Failed openning /sys/class/fw/rules/rules."
#define MAIN_RULE_TABLE_WRITING_ATTRIBUTE_ERR_MSG "Writing to ./sys/class/fw/rules/rules, the program failed."
#define SRTCMP_OF_EQ 0

#define MAIN_ERR_CHECK(condition, extra_code){  \
    if(condition){                              \
        extra_code                              \
        return EXIT_FAILURE;                    \
    }                                           \
}

#define MAIN_SIMPLE_ERR_CHECK(condition) MAIN_ERR_CHECK(condition,)
#define MAIN_MSG_ERR_CHECK(condition, extracode, msg) MAIN_ERR_CHECK(condition, extracode; printf("%s\n", msg);)

#endif