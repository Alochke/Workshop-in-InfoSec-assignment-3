#ifndef LOGS_LIST
#define LOGS_LIST

#include "fw.h"
#include "list.h"

void logs_get(struct klist_node*);


/*
    Returns a pointer to the log_row_t that is also a member of the node that the klist_node pointed by knode is a member of.

    Paremeters:
    - knode: The klist_node that shares the containing node with the extracted log_row_t.
*/
static inline log_row_t* node_to_log(struct klist_node *knode)
{
    return (log_row_t*)(((node*)knode)->content);
}
#endif