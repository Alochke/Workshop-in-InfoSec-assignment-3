#include "list.h"

#define SIZE_PLUS_NULL(x) (strlen(x) + 1)

/*
	Initializes l.

	Parameters:
	- l: A pointer to a pointer that will point to the initialized list.
*/
int list_init(list **l)
{
	MAIN_SIMPLE_ERR_CHECK((*l = malloc(sizeof(list))) == NULL)
	(*l)->head = NULL;
	(*l)->size = 0;
	return EXIT_SUCCESS;
}

/*
	Inserts node into l from the head-end of l.

	Parameters:
	- l: The address of the list node will be inserted into.
	- node: The address of the inserted node.

	Returns: node's address, or NULL if the initialization failed.
*/
list_node *list_insert(list *l, list_node *node)
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
	Inserts a node that will be initialized with key as its key, to the head-end of l.

	Parameters:
	- l: The address of the list we'll insert the key into.
	- key: The string that node->key will point to.

	Returns: A pointer with the inserted node's address, or NULL if the initialization failed.
*/
list_node *list_insert_key(list *l, char *key)
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
	Frees l and all of its nodes.

	Parameters:
	- l: A pointer to the freed list.
*/
void list_destroy(list *l)
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
	free(l);
}

/*
	Reverses *l.

	Parameters:
	- l: A pointer to the reversed list.
*/
void list_reverse(list *l)
{
	list_node *prev = NULL;
	list_node *curr = l->head;
	list_node *next;
	while (curr != NULL)
	{
		next = curr->next;
		curr->next = prev;
		curr->prev = next;
		prev = curr;
		curr = next;
	}
	l->head = prev;
}

/*
	Parses stream into a list where every node has a line of stream as its key.
	
	Assumptions: stream wasn't read beforehand, and *l is empty.

	Parameters:
	- l: Address of the list that will have all of stream's lines stored as the keys of its nodes.
	- stream: The address of the FILE we'll read the lines from.

	Returns: 0 on success, 1 upon failure.
*/
int list_parse_lines(list *l, FILE *stream)
{
    char *datapoint_line = NULL;

	while (getline(&datapoint_line, NULL, stream) != EOF)
	{
		char *temp = (char *)malloc(sizeof(char) * SIZE_PLUS_NULL(datapoint_line));
		
		MAIN_SIMPLE_ERR_CHECK(temp == NULL)
		
		strcpy(temp, datapoint_line);
		MAIN_ERR_CHECK(list_insert_key(l, temp) == NULL, free(temp);)
	}
	free(datapoint_line);
	return EXIT_SUCCESS;
}
