#ifndef LOGS_LIST
#define LOGS_LIST

#include "fw.h"
#include "list.h"

void list_get(struct klist_node*);


/*
    Returns a pointer to the log_row_t that is also a member of the node that knode is a member of.
*/
static inline log_row_t* node_to_log(struct klist_node *knode)
{
    return (log_row_t*)(((node*)knode)->content);
}
#endif