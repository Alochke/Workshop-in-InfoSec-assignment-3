#ifndef RULE_TABLE_PARSER
#define RULE_TABLE_PARSER
#include <unistd.h>
#include <stdlib.h>
#include "linked_list.h"
#include "fw.h"
#include "main.h"

#define RULE_TABLE_PARSER_IN_LINKED_LIST_EXIT_FAILURE -1

int rule_table_parser_in_line_linked_list(linked_list *, FILE *stream);

#endif
