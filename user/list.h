#ifndef LINKEDLIST
#define LINKEDLIST
#include <stdio.h>
#include <stdlib.h>
#include "main.h"
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
int list_init(list *);
list_node *list_insert(list *, list_node *);
list_node *list_insert_key(list *, char *);
void list_delete(list *, list_node *);
void list_reverse(list *);
void list_destroy(list *);
#endif
