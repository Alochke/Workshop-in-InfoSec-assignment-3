#ifndef MAIN
#define MAIN

#include <string.h> // For strcmp.
#include <unistd.h> // For read and write.
#include <fcntl.h> // For openning files.
#include "list.h"
#include "rule_table.h"
#include "logs.h"

#define MAIN_ARG_ERR_MSG "Wrong arguments, you've entered.\nThe correct arguments are:\n•show_rules\n•load_rules <path_to_rules_file>\n•show_log\n•clear_log"
#define MAIN_FILE_OPEN_ERR_MSG "Wrong file path, you've given. Or permissions for it, you lack."
#define MAIN_MALLOC_ERR_MSG "Failed an allocation, the program."
#define MAIN_FILE_FORMAT_ERR_MSG "Wrong format, the file you've given is in."
#define MAIN_RULE_TABLE_OPEN_ATTRIBUTE_ERR_MSG "Openning /sys/class/fw/rules/rules, the program has failed."
#define MAIN_RULE_TABLE_WRITE_ATTRIBUTE_ERR_MSG "Writing to /sys/class/fw/rules/rules, the program has failed."
#define MAIN_RULE_TABLE_READ_ATTRIBUTE_ERR_MSG "Reading from /sys/class/fw/rules/rules, the program has failed."
#define MAIN_LOGS_CHAR_DEV_OPEN_ERR_MSG "Openning /dev/fw_log, the program has failed."
#define MAIN_READ_LOGS_ERR_MSG "Occurred, an error, when from /dev/fw_log- the program read."
#define MAIN_LOCALTIME_R_ERR_MSG "Has failed, the function localtime_r."
#define MAIN_LOGS_ATTRIBUTE_OPEN_ERR_MSG "Occurred, an error, when /sys/class/fw/log/reset, the program openned."
#define MAIN_LOGS_ATTRIBUTE_WRITE_ERR_MSG "Occurred, an error, when to /sys/class/fw/log/reset, the program wrote."
#define SRTCMP_OF_EQ 0
#define MAIN_SYSCALL_FAIL_RETURN -1
#define MAIN_NULL_INCLUDED 1 // Used to signify the reason we're adding one to the length of a string that will store deserialized struct members.

#define MAIN_MAX_INDX_OF_BYTE_IN_ADDRESS 3

#define MAIN_ERR_CHECK(condition, extra_code){  \
    if(condition){                              \
        extra_code                              \
        return EXIT_FAILURE;                    \
    }                                           \
}

#define MAIN_SIMPLE_ERR_CHECK(condition) MAIN_ERR_CHECK(condition,)
#define MAIN_MSG_ERR_CHECK(condition, extracode, msg) MAIN_ERR_CHECK(condition, extracode; fprintf(stderr, "%s\n", msg);)

int main_deseralize_field(char**, unsigned int, char**, unsigned int*, size_t);

#endif