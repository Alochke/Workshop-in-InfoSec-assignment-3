#ifndef LIST
#define LIST

#include "fw.h"
#include "list.h"

void list_get(struct klist_node*);

/*
    Returns a pointer to the connection_table_entry that is also a member of the node that knode is a member of.
*/
static inline connection_table_entry* node_to_log(struct klist_node *knode)
{
    return (connection_table_entry*) (((node*)knode)->content);
}
#endif