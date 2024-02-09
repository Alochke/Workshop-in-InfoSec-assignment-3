#ifndef LOGS
#define LOGS

#include <linux/kernel.h>   /* We're doing kernel work. */
#include <linux/slab.h> // The include is for kmalloc.
#include <linux/timekeeping.h> // For time stamping the logs.
#include "fw.h"
#include "main.h"
#inlude "list.h"

int logs_init(void);
void logs_destroy(void);
ssize_t logs_read(struct file *filp, char *buff, size_t length, loff_t *offp);
int logs_open(struct inode *_inode, struct file *_file);
void logs_update(unsigned char protocol, unsigned char action, __be32 src_ip, __be32 dst_ip, __be16 src_port, __be16 dst_port, reason_t reason);


#endif