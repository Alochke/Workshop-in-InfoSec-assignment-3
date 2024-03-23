#ifndef LIST
#define LIST

#include "fw.h"
#include "connection_table.h"

void list_put(struct klist_node*);
void list_get(struct klist_node*);
void list_destroy(struct klist*, struct klist_iter*);

typedef struct log_node{
    struct klist_node node;
    struct connection_table_entry entry;
}log_node;

/*
    Returns the connection_table_entry that is also a member of the log_node that klist is a member of.
*/
static inline connection_table_entry node_to_log(struct klist_node *knode)
{
    return (((log_node*)knode)->entry);
}
#endif