#ifndef LOGS
#define LOGS

#include <linux/kernel> // The include is for kernel work.
#include <linux/slab.h> // The include is for kmalloc.
#include <linux/timekeeping.h> // For time stamping the logs.
#include "fw.h"
#include "main.h"

int logs_init(void);
void logs_destroy(void);
ssize_t logs_read(struct file *filp, char *buff, size_t length, loff_t *offp);
int logs_open(struct inode *_inode, struct file *_file);
void logs_update(unsigned char, unsigned char, __be32, __be32, __be16, __be16, reason_t)


#endif