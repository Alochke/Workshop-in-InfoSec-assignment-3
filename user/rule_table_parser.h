#ifndef RULE_TABLE_PARSER
#define RULE_TABLE_PARSER
#include <unistd.h>
#include <stdlib.h>
#include "list.h"
#include "fw.h"
#include "main.h"

#define rule_table_parser_list_lines_EXIT_FAILURE -1

int rule_table_parser_list_lines(list *, FILE *stream);
int rule_table_parser_in_init(rule_t*, list*);

#endif
