#ifndef CONNECTION_TABLE_LIST
#define CONNECTION_TABLE_LIST

#include "fw.h"
#include "list.h"

void connection_table_get(struct klist_node*);

/*
    Returns a pointer to the connection_table_entry that is also a member of the node that the klist_node pointed by knode is a member of.

    Paremeters:
    - knode: The klist_node that shares the containing node with the extracted connection_table_entry.
*/
static inline connection_table_entry* node_to_connection_table_entry(struct klist_node *knode)
{
    return (connection_table_entry*) (((node*)knode)->content);
}
#endif