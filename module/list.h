#include <linux/klist.h>    /* We're building the list on top of the klist implementation within the source tree.*/
#include <linux/kernel.h>   /* We're doing kernel work. */
#include "fw.h"

void put(struct klist_node*);
void list_destroy(struct klist*, struct klist_iter*)

typedef struct log_node{
    struct klist_node node;
    struct log_row_t* log;
};

static inline log_row_t* node_to_log(klist_node *node)
{
    return ((log_node*)node)->log;
}