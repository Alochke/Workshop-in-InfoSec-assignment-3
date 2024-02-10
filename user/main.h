#ifndef MAIN
#define MAIN

#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "list.h"
#include "rule_table.h"
#include "logs.h"

#define MAIN_ARG_ERR_MSG "Wrong arguments, you've entered.\nThe correct arguments are:\n•show_rules\n•load_rules <path_to_rules_file>\n•show_log\n•clear_log"
#define MAIN_FILE_OPEN_ERR_MSG "Wrong file path, you've given. Or permissions for it, you lack."
#define MAIN_MALLOC_ERR_MSG "Failed, the program an allocation."
#define MAIN_FILE_FORMAT_ERR_MSG "Wrong format, the file you've given is in."
#define MAIN_RULE_TABLE_OPENNING_ATTRIBUTE_ERR_MSG "Failed openning /sys/class/fw/rules/rules."
#define MAIN_RULE_TABLE_WRITING_ATTRIBUTE_ERR_MSG "Writing to /sys/class/fw/rules/rules, the program failed."
#define MAIN_SHOW_LOGS_FILE_OPEN "Failed is the /dev/fw_log openning."
#define SRTCMP_OF_EQ 0
#define MAIN_SYSCALL_FAIL_RETURN -1

#define MAIN_DIRECTION_STRS (char*[DIRECTION_NUM]){"in", "out", "any"} // The direction specifications that can appear in a rule table configuration file.
#define MAIN_DIRECTION_VALS (unsigned int[DIRECTION_NUM]){DIRECTION_IN, DIRECTION_OUT, DIRECTION_ANY} // The possible values of a direction member of a rule_t.
#define MAIN_PROT_STRS (char*[PROTS_NUM]){"TCP", "UDP", "ICMP", "any"} // The protocol specifications that can appear in a rule table configuration file.
#define MAIN_PROT_VALS (unsigned int[PROTS_NUM]){PROT_TCP, PROT_UDP, PROT_ICMP, PROT_ANY} // The legal values of a protocol member of a rule_t.
#define MAIN_ACK_STRS (char*[ACK_NUM]){"yes", "no", "any"} // The ack relevent ack configuration that can appear in a rule table configuration file.
#define MAIN_ACK_VALS (unsigned int[ACK_NUM]){ACK_YES, ACK_NO, ACK_ANY} // The possible values of an ack member of a rule_t.
#define MAIN_ACTION_STRS (char*[ACTIONS_NUM]){"accept\r\n", "accept\n", "drop\r\n", "drop\n"} // The possible strings for an accepetion/dropping verdict configuration within a rule table configuration file that doesn't appear at the end of the file.
#define MAIN_ACTION_STRS (unsigned int[ACTIONS_NUM]){NF_ACCEPT, NF_ACCEPT, NF_DROP, NF_DROP} // The possible values of the action member of a rule_t, respectively to the string that appear in ACTION_STRS.
#define MAIN_END_ACTION_STRS (char*[END_ACTIONS_NUM]){"accept", "drop"} // The possible strings for an accepetion/dropping verdict configuration within a rule table configuration file that appear at the end of the file.
#define MAIN_END_ACTION_VALS (unsigned int[END_ACTIONS_NUM]){NF_ACCEPT, NF_DROP} // The possible values of the action member of a rule_t, respectively to the string that appear in END_ACTION_STRS.

#define MAIN_ERR_CHECK(condition, extra_code){  \
    if(condition){                              \
        extra_code                              \
        return EXIT_FAILURE;                    \
    }                                           \
}

#define MAIN_SIMPLE_ERR_CHECK(condition) MAIN_ERR_CHECK(condition,)
#define MAIN_MSG_ERR_CHECK(condition, extracode, msg) MAIN_ERR_CHECK(condition, extracode; fprintf(stderr, "%s\n", msg);)

#endif