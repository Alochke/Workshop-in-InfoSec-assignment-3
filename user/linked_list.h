#ifndef LINKEDLIST
#define LINKEDLIST
#include <stdio.h>
#include <stdlib.h>
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
} linked_list;
void list_init(linked_list *);
list_node *list_insert(linked_list *, list_node *);
list_node *list_insert_key(linked_list *, char *);
void list_delete(linked_list *, list_node *);
void list_reverse(linked_list *);
void list_destroy(linked_list *);
#endif
