#ifndef RULE_TABLE
#define RULE_TABLE

#include "fw.h"
#include "main.h"
#include <linux/uaccess.h>

int rule_table_init(void);
void rule_table_destroy(void);

#endif
