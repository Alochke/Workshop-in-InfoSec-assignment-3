#ifndef CONNECTION_TABLE
#define CONNECTION_TABLE

#include "fw.h"
#include "main.h"

typedef enum state{
	LISTEN,
    LISTEN_SYN_RCVD,
    SYN_RCVD,
    SYN_SENT,
    SYN_SENT_ESTABLISHED,
    SYN_SENT_RCVD,
    ESTABLISHED,
    ESTABLISHED_CLOSE_WAIT,
    CLOSE_WAIT,
    LAST_ACK,
    FIN_WAIT_1,
    FIN_WAIT_1_CLOSING,
    FIN_WAIT_1_TIME_WAIT,
    CLOSING,
    TIME_WAIT,
    FIN_WAIT_2,
    FIN_WAIT_2_TIME_WAIT
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