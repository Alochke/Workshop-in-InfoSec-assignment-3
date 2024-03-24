#ifndef _FW_H_
#define _FW_H_

#include <linux/kernel.h>   /* We're doing kernel work. */
#include <linux/slab.h>     /* For kmaloc and kfree. */
#include <linux/netfilter.h>/* Next two includes are for the Netfilter API */
#include <linux/netfilter_ipv4.h>
#include <linux/string.h> // For strcmp.
#include <linux/ip.h> // The three next includes are for inspecting the traffic.
#include <linux/tcp.h>
#include <linux/udp.h>
#include <linux/timekeeping.h> // For time stamping the logs.
#include <linux/uaccess.h> // For copy_to_user.
#include <linux/fs.h> /* This and the next header are for sysfs kernel api to work. */
#include <linux/device.h> 

#define FW_PORT_ANY			(0)


// the protocols we will work with
typedef enum {
	PROT_ICMP	= 1,
	PROT_TCP	= 6,
	PROT_UDP	= 17,
	PROT_ANY	= 143
} fw_prot_t;


// various reasons to be registered in each log entry
typedef enum {
	REASON_NO_MATCHING_RULE      = -2,
	REASON_XMAS_PACKET           = -4
} reason_t;

// device minor numbers, for your convenience
typedef enum main_minors{
    RULE_TABLE_MINOR,
    LOGS_DEV_MINOR,
    LOGS_SYSFS_MINOR,
	CONNECTION_TABLE_MINOR
} fw_minor_t;

typedef enum {
	ACK_NO 		= 0x01,
	ACK_YES 	= 0x02,
	ACK_ANY 	= ACK_NO | ACK_YES,
} ack_t;

typedef enum {
	DIRECTION_IN 	= 0x01,
	DIRECTION_OUT 	= 0x02,
	DIRECTION_ANY 	= DIRECTION_IN | DIRECTION_OUT,
} direction_t;

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

// rule base
typedef struct {
	char rule_name[20];			// names will be no longer than 20 chars
	direction_t direction;
	__be32	src_ip;
	__be32	src_prefix_mask; 	// e.g., 255.255.255.0 as int in the local endianness
	__u8    src_prefix_size; 	// valid values: 0-32, e.g., /24 for the example above
								// (the field is redundant - easier to print)
	__be32	dst_ip;
	__be32	dst_prefix_mask; 	// as above.
	__u8    dst_prefix_size; 	// as above.
	__be16	src_port; 			// number of port or 0 for any or port 1024 for any port number > 1023  
	__be16	dst_port; 			// number of port or 0 for any or port 1024 for any port number > 1023 
	__u8	protocol; 			// values from: prot_t
	ack_t	ack; 				// values from: ack_t
	__u8	action;   			// valid values: NF_ACCEPT, NF_DROP
} rule_t;

// logging
typedef struct {
	unsigned long  	timestamp;     	// time of creation/update
	unsigned char  	protocol;     	// values from: prot_t
	unsigned char  	action;       	// valid values: NF_ACCEPT, NF_DROP
	__be32   		src_ip;		  	// if you use this struct in userspace, change the type to unsigned int
	__be32			dst_ip;		  	// if you use this struct in userspace, change the type to unsigned int
	__be16 			src_port;	  	// if you use this struct in userspace, change the type to unsigned short
	__be16 			dst_port;	  	// if you use this struct in userspace, change the type to unsigned short
	reason_t     	reason;       	// rule#index, or values from: reason_t
	unsigned int   	count;        	// counts this line's hits
} log_row_t;

typedef struct connection_table_entry
{
    __be32  src_ip;
    __be16	src_port;
    __be32  dst_ip;
    __be16	dst_port;
    state  cur_state;
}connection_table_entry;

#endif // _FW_H_