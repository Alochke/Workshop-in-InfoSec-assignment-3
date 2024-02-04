#ifndef _FW_H_
#define _FW_H_

// the protocols we will work with
typedef enum {
	PROT_ICMP	= 1,
	PROT_TCP	= 6,
	PROT_UDP	= 17,
	PROT_OTHER 	= 255,
	PROT_ANY	= 143
} fw_prot_t;


// various reasons to be registered in each log entry
typedef enum {
	REASON_FW_INACTIVE           = -1,
	REASON_NO_MATCHING_RULE      = -2,
	REASON_XMAS_PACKET           = -4,
	REASON_ILLEGAL_VALUE         = -6,
} reason_t;
	
#define FW_DEVICE_NAME_CONN_TAB		"conn_tab"
#define FW_LOOPBACK_NET_DEVICE_NAME	"lo"
#define FW_IN_NET_DEVICE_NAME		"eth1"
#define FW_OUT_NET_DEVICE_NAME		"eth2"

// auxiliary values, for your convenience
#define FW_IP_VERSION		(4)
#define FW_PORT_ANY			(0)
#define FW_PORT_ABOVE_1023	1024 // The value of a port member in a rule_t in case the rule_t represents a rule where a matching port number is every port above 1023

// device minor numbers, for your convenience
typedef enum {
	MINOR_RULES    = 0,
	MINOR_LOG      = 1,
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

// rule base
typedef struct {
	char 			rule_name[20];		// names will be no longer than 20 chars
	direction_t 	direction;
	unsigned int 	src_ip;
	unsigned int 	src_prefix_mask;	// e.g., 255.255.255.0 as int in the local endianness
	unsigned char 	src_prefix_size;	// valid values: 0-32, e.g., /24 for the example above
										// (the field is redundant - easier to print)
	unsigned int	dst_ip;
	unsigned int 	dst_prefix_mask; 	// as above
	unsigned char   dst_prefix_size; 	// as above	
	unsigned short	src_port;			// number of port or 0 for any or port 1024 for any port number > 1023  
	unsigned short	dst_port; 			// number of port or 0 for any or port 1024 for any port number > 1023 
	unsigned char	protocol; 			// values from: prot_t
	ack_t			ack; 				// values from: ack_t
	char			action;   			// valid values: NF_ACCEPT, NF_DROP
} rule_t;

// logging
typedef struct {
	unsigned long  	timestamp;     	// time of creation/update
	unsigned char  	protocol;     	// values from: prot_t
	unsigned char  	action;       	// valid values: NF_ACCEPT, NF_DROP
	unsigned int   	src_ip;		  	// if you use this struct in userspace, change the type to unsigned int
	unsigned int	dst_ip;		  	
	unsigned short 	src_port;	  	// if you use this struct in userspace, change the type to unsigned short
	unsigned short	dst_port;	  	
	reason_t     	reason;       	// rule index, or values from: reason_t
	unsigned int   	count;        	// counts this line's hits
} log_row_t;

#endif // _FW_H_