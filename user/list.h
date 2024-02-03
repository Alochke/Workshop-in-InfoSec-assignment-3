#ifndef LIST
#define LIST
#include <stdio.h>
#include <stdlib.h>

#define LIST_LAST_NODE_INDX(list) (list->size - 1)

typedef struct list_node
{
	char *key;
	struct list_node *prev;
	struct list_node *next;
} list_node;

typedef struct
{
	unsigned int size;
	list_node *head;
} list;

#include "main.h" // This line is here intentionally, otherwise "rule_table.h" would be included without the typedefs it needs to pass pre-processing.

int list_init(list **);
list_node *list_insert(list *, list_node *);
list_node *list_insert_key(list *, char *);
void list_delete(list *, list_node *);
void list_reverse(list *);
void list_destroy(list *);
 
#endif
