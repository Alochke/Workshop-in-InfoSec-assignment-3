#include "logs.h"

// The next format strings are taking into acount the maximal lengthes of fields of a row within the output of read_log, and will be used to make a format string for that row.
#define TIMESTAMP_FORMAT "%-19s "
#define IP_FORMAT "%-15s "
#define PORT_FORMAT "%-8s "
#define PROT_FORMAT "%-8s "
#define ACTION_FORMAT "%-6s "
#define REASON_FORMAT "%-23s "
#define COUNT_FORMAT "%-10s"
#define LOG_FORMAT_STRING TIMESTAMP_FORMAT IP_FORMAT IP_FORMAT PORT_FORMAT PORT_FORMAT PROT_FORMAT ACTION_FORMAT REASON_FORMAT COUNT_FORMAT "\n"
#define MAX_TIMESTAMP_LEN 19
#define MAX_IP_LEN 15
#define MAX_PORT_LEN 5
#define MAX_COUNT_LEN 10
#define MAX_REASON_LEN 2 // This is diffrent than the suggested length by REASON_FORMAT because this will be used only if the verdict logged is by a rule table row, and the maximal index of such is 50, which is 2 digits long.
#define TM_MON_OFFSET 1 // This signifies that tms start to count the monthes from 0.
#define TM_YEAR_OFFSET 1900 // This signifies that tms start to count the years from 1900.

#define PROTS_NUM 3
#define PROT_STRS (char*[PROTS_NUM]){"tcp", "udp", "icmp"} // The protocol specifications that can appear in a log row.
#define PROT_VALS (unsigned int[PROTS_NUM]){PROT_TCP, PROT_UDP, PROT_ICMP} // The legal values of a protocol member of a log_row_t, respectively to the strings that appear in PROT_STRS.
#define ACTIONS_NUM 2
#define ACTION_STRS (char*[ACTIONS_NUM]){"accept", "drop"} // The possible strings for packet accepetion/dropping verdict that can appear in a log row.
#define ACTION_VALS (unsigned int[ACTIONS_NUM]){NF_ACCEPT, NF_DROP} // The possible values of the action member of a log_row_t, respectively to the strings that appear in ACTION_STRS.
#define REASONS_NUM 2
#define REASON_STRS (char*[REASONS_NUM]){"REASON_NO_MATCHING_RULE", "REASON_XMAS_PACKET"} // The possible strings for the reason of acception/dropping that can appear in a log row, that are not integers.
#define REASON_VALS (unsigned int[REASONS_NUM]){REASON_NO_MATCHING_RULE, REASON_XMAS_PACKET} // The possible values of the reason member of a log_row_t, respectively to the strings that appear in REASON_STRS.

/*
	Reads the logs from a file descriptor and after that prints the logs.
		
	Assumptions: fd is a read-permitted file descriptor of /dev/fw_log, that has its offset set to 0.

	Parameters:
	- fd: A file descriptor that's supposed to be a read-permitted file descriptor of /dev/fw_log, and have its offset set to 0.

	Returns: 1 on success, 0 on failure.
*/
int logs_read(int fd)
{
    unsigned int row_num;
    log_row_t* rows;

    // It's ok to use the read system call like this because we know how the read implementation of this char dev is implemented.
    MAIN_MSG_ERR_CHECK(read(fd, &row_num, sizeof(unsigned int)) != sizeof(unsigned int),, MAIN_READ_LOGS_ERR_MSG);
    rows = malloc(sizeof(log_row_t) * row_num);
    MAIN_MSG_ERR_CHECK(rows == NULL,, MAIN_MALLOC_ERR_MSG);
    MAIN_MSG_ERR_CHECK(read(fd, rows, sizeof(log_row_t) * row_num) != (sizeof(log_row_t) * row_num), free(rows), MAIN_READ_LOGS_ERR_MSG);

    printf(LOG_FORMAT_STRING, "timestamp", "src_ip", "dst_ip", "src_port", "dst_port", "protocol", "action", "reason", "count");

    for (size_t i = 0; i < row_num; i++)
    {
        char timestamp[MAX_TIMESTAMP_LEN + MAIN_NULL_INCLUDED], src_ip[MAX_IP_LEN + MAIN_NULL_INCLUDED], dst_ip[MAX_IP_LEN + MAIN_NULL_INCLUDED], src_port[MAX_PORT_LEN + MAIN_NULL_INCLUDED], dst_port[MAX_PORT_LEN + MAIN_NULL_INCLUDED], count[MAX_COUNT_LEN + MAIN_NULL_INCLUDED], reason_num[MAX_REASON_LEN];
        char *protocol, *action, *reason;
        reason = reason_num;
        time_t walltime = rows[i].timestamp;
        struct tm tm_instance;
        MAIN_MSG_ERR_CHECK(localtime_r(&walltime, &tm_instance) == NULL, free(rows), MAIN_LOCALTIME_R_ERR_MSG)
        snprintf(timestamp, MAX_TIMESTAMP_LEN + MAIN_NULL_INCLUDED, "%02d/%02d/%d %02d:%02d:%02d", tm_instance.tm_mday, tm_instance.tm_mon + TM_MON_OFFSET, tm_instance.tm_year + TM_YEAR_OFFSET, tm_instance.tm_hour, tm_instance.tm_min, tm_instance.tm_sec);
        snprintf(src_ip, MAX_IP_LEN + MAIN_NULL_INCLUDED, "%d.%d.%d.%d", ((unsigned char*) &rows[i].src_ip)[MAIN_MAX_INDX_OF_BYTE_IN_ADDRESS], ((unsigned char*) &rows[i].src_ip)[MAIN_MAX_INDX_OF_BYTE_IN_ADDRESS - 1], ((unsigned char*) &rows[i].src_ip)[MAIN_MAX_INDX_OF_BYTE_IN_ADDRESS - 2], ((unsigned char*) &rows[i].src_ip)[MAIN_MAX_INDX_OF_BYTE_IN_ADDRESS - 3]);
        snprintf(dst_ip, MAX_IP_LEN + MAIN_NULL_INCLUDED, "%d.%d.%d.%d", ((unsigned char*) &rows[i].dst_ip)[MAIN_MAX_INDX_OF_BYTE_IN_ADDRESS], ((unsigned char*) &rows[i].dst_ip)[MAIN_MAX_INDX_OF_BYTE_IN_ADDRESS - 1], ((unsigned char*) &rows[i].dst_ip)[MAIN_MAX_INDX_OF_BYTE_IN_ADDRESS - 2], ((unsigned char*) &rows[i].dst_ip)[MAIN_MAX_INDX_OF_BYTE_IN_ADDRESS - 3]);
        snprintf(src_port, MAX_PORT_LEN + MAIN_NULL_INCLUDED, "%d", rows[i].src_port);
        snprintf(dst_port, MAX_PORT_LEN + MAIN_NULL_INCLUDED, "%d", rows[i].dst_port);
        main_deserialize_field(&protocol, rows[i].protocol, PROT_STRS, PROT_VALS, PROTS_NUM);
        main_deserialize_field(&action, rows[i].action, ACTION_STRS, ACTION_VALS, ACTIONS_NUM);
        snprintf(count, MAX_COUNT_LEN + MAIN_NULL_INCLUDED, "%d", rows[i].count);
        if(main_deserialize_field(&reason, rows[i].reason, REASON_STRS, REASON_VALS, REASONS_NUM))
        {
            snprintf(reason_num, MAX_REASON_LEN, "%d", rows[i].reason);
        }
        printf(LOG_FORMAT_STRING, timestamp, src_ip, dst_ip, src_port, dst_port, protocol, action, reason, count);
    }
    free(rows);
    return EXIT_SUCCESS;
}