#ifndef RULE_TABLE
#define RULE_TABLE
#include <unistd.h>
#include <stdlib.h>
#include <arpa/inet.h> // For htol/htoc.
#include <linux/netfilter.h>
#include <stdbool.h>
#include <stdio.h> // For snprintf.
#include "list.h"
#include "fw.h"
#include "main.h"

#define rule_table_list_lines_EXIT_FAILURE -1

int rule_table_list_lines(list*, FILE*);
int rule_table_in_init(rule_t**, list*);
int rule_table_out_print(FILE*);

#endif
