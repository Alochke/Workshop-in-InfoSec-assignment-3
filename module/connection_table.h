#ifndef CONNECTION_TABLE
#define CONNECTION_TABLE

#include "fw.h"
#include "main.h"

typedef enum state{
    /*
    The reason I added the "STATE_" prefix is that apperntly TIME_WAIT is already defined,
    within the kernel source.
    */
	STATE_LISTEN,
    STATE_LISTEN_SYN_RCVD,
    STATE_SYN_RCVD,
    STATE_SYN_SENT,
    STATE_SYN_SENT_ESTABLISHED,
    STATE_SYN_SENT_RCVD,
    STATE_ESTABLISHED,
    STATE_ESTABLISHED_CLOSE_WAIT,
    STATE_CLOSE_WAIT,
    STATE_LAST_ACK,
    STATE_FIN_WAIT_1,
    STATE_FIN_WAIT_1_CLOSING,
    STATE_FIN_WAIT_1_TIME_WAIT,
    STATE_CLOSING,
    STATE_TIME_WAIT,
    STATE_FIN_WAIT_2,
    STATE_FIN_WAIT_2_TIME_WAIT
} state;

typedef struct connection_table_entry
{
    __be32  src_ip;
    __be16	src_port;
    __be32  dst_ip;
    __be16	dst_port;
    state  cur_state;
}connection_table_entry;

int connection_table_init(void);
void connection_table_destroy(void);

#endif