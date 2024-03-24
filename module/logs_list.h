#ifndef LIST
#define LIST

#include "fw.h"
#include "list.h"

void list_put(struct klist_node*);
void list_get(struct klist_node*);

typedef struct{
    struct klist_node node;
    struct log_row_t* log;
}log_node;

/*
    Returns a pointer to the log_row_t that is also a member of the log_node that klist is a member of.
*/
static inline log_row_t* node_to_log(struct klist_node *knode)
{
    return (log_row_t*)(((log_node*)knode)->log);
}
#endif