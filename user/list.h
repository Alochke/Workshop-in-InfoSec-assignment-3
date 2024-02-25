#ifndef LIST
#define LIST

#include "fw.h"

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

int list_init(list **);
void list_reverse(list *);
void list_destroy(list *);
int list_parse_lines(list *l, FILE *stream);

#include "main.h" // This is here intentionaly, else, we'll have an unknown type error on compilation.

#endif
