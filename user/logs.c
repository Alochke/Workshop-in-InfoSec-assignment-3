#include "logs.h"

#define READ_ERR_MSG "To read from /dev/fw_log, the program has failed."

// The next format strings are taking into acount the maximal lengthes of fields of a row within the output of read_log, and will be used to make a format string for that row.
#define TIMESTAMP_FORMAT "%-19s "
#define IP_FORMAT "%-15s "
#define PORT_FORMAT "%-8s "
#define PROTOCOL_FORMAT "%-8s "
#define ACTION_FORMAT "%-6s "
#define REASON_FORMAT "%-23s "
#define COUNT_FORMAT "%-10s "
#define LOG_FORMAT_STRING TIMESTAMP_FORMAT IP_FORMAT IP_FORMAT PORT_FORMAT PORT_FORMAT PROTOCOL_FORMAT ACTION_FORMAT REASON_FORMAT COUNT_FORMAT "\n"

int read_logs(int fd)
{
    unsigned long row_num;
    log_row_t* buff;

    MAIN_MSG_ERR_CHECK(read(fd, &row_num, sizeof(unsigned long)) == MAIN_SYSCALL_FAIL_RETURN,, READ_ERR_MSG);
    buff = malloc(sizeof(log_row_t) * row_num);
    MAIN_MSG_ERR_CHECK(buff == NULL,, MAIN_MALLOC_ERR_MSG);
    MAIN_MSG_ERR_CHECK(read(fd, buff, sizeof(log_row_t) * row_num) != (sizeof(log_row_t) * row_num), free(buff), READ_ERR_MSG)

    printf(LOG_FORMAT_STRING, "timestamp", "src_ip", "dst_ip", "src_port", "dst_port", "protocol", "action", "reason", "count");

    for (size_t i = 0; i < row_num; i++)
    {
        /* code */
    }
    
}