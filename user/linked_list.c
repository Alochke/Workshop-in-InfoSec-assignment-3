#include "linked_list.h"
/*
	Initializes l.

	@l: The initalized list.
*/
void list_init(linked_list *l)
{
	l->head = NULL;
	l->size = 0;
}
/*
	Inserts node into l from the head-end of l.

	@l: The list to which l will be inserted.
	@node: The which will be inserted.

	returns: A pointer with node's address.
*/
list_node *list_insert(linked_list *l, list_node *node)
{
	node->next = l->head;
	if (l->head != NULL)
	{
		l->head->prev = node;
	}
	l->head = node;
	node->prev = NULL;
	l->size++;
	return node;
}
/*
	Inserts to l a node that will be initalized with key as its key.

	@l: The list we'll insert the key to.
	@key: This will be the key of the isnerted node.

	returns: A pointer with the insrted node's address.
*/
list_node *list_insert_key(linked_list *l, char *key)
{
	list_node *node = (list_node *)malloc(sizeof(list_node));
	if (node == NULL)
	{
		return NULL;
	}
	node->key = key;

	return list_insert(l, node);
}
/*
	Removes p from l.

	@l: p's list.
	@p: The node we'll remove from l.
*/
void list_delete(linked_list *l, list_node *p)
{
	if (p->prev != NULL)
	{
		(p->prev)->next = p->next;
	}
	else
	{
		l->head = p->next;
	}
	if (p->next != NULL)
		(p->next)->prev = p->prev;
	l->size--;
}
/*
	Frees l and all of its nodes.

	@l: The freed list.
*/
void list_destroy(linked_list *l)
{
	list_node *temp;
	if(l==NULL)
	{
		return;
	}
	while (l->head != NULL)
	{
		temp = l->head;
		l->head = l->head->next;
		free(temp->key);
		free(temp);
	}
}
/*
	Reverses l.

	@l: The reversed list.
*/
void list_reverse(linked_list *l)
{
	list_node *prev = NULL;
	list_node *curr = l->head;
	list_node *next;
	while (curr != NULL)
	{
		next = curr->next;
		curr->next = prev;
		prev = curr;
		curr = next;
	}
	l->head = prev;
}


