#ifndef LOGS
#define LOGS

#include "fw.h"
#include "main.h"

int logs_init(void);
void logs_destroy(void);
ssize_t logs_read(struct file *filp, char *buff, size_t length, loff_t *offp);
int logs_open(struct inode *_inode, struct file *_file);



#endif