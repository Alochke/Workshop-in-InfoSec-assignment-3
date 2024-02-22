#ifndef RULE_TABLE
#define RULE_TABLE

#include <linux/ip.h> // For hton and ntoh.
#include "main.h"
#include "fw.h"

extern unsigned char rule_table_rules_num;
extern rule_t* rule_table;

int rule_table_init(void);
void rule_table_destroy(void);

#endif
