/*
    The list h and c files were made to eliminate code duplication.
    The list_destroy and list_put function of logs_list and connection_table_list look exactly the same.
    So here we store code that is suitable for both.
    We also implement here a general node to be used as the node of both lists.
*/

#ifndef LIST
#define LIST

void list_destroy(struct klist *list, struct klist_iter *iter);
void list_put(struct klist_node* node);

typedef struct node{
    struct klist_node _node;
    struct void* content;
}node;

#endif