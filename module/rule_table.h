#ifndef RULE_TABLE
#define RULE_TABLE

#include "fw.h"
#include "main.h"
#include <linux/ip.h> // For hton and ntoh.

extern unsigned char rule_table_rules_num;
extern rule_t* rule_table;

int rule_table_init(void);
void rule_table_destroy(void);

#endif
