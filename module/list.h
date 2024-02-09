#ifndef LIST
#define LIST

#include <linux/klist.h>    /* We're building the list on top of the klist implementation within the source tree.*/
#include <linux/kernel.h>   /* We're doing kernel work. */
#include <linux/slab.h>     /* For allocation and deallocation. */
#include "fw.h"

void list_put(struct klist_node*);
void list_get(struct klist_node*);
void list_destroy(struct klist*, struct klist_iter*);

typedef struct{
    struct klist_node node;
    struct log_row_t* log;
}log_node;

static inline log_row_t* node_to_log(struct klist_node *knode)
{
    return (log_row_t*)(((log_node*)knode)->log);
}
#endif