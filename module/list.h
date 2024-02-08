#include <linux/klist.h>   /* We're building the list on top of the klist implementation within the source tree.*/
#include <linux/kernel.h>   /* We're doing kernel work. */
#include "fw.h"

void put(struct klist_node*);
void list_destroy(struct klist*, struct klist_iter*);

struct{
    struct klist_node node;
    struct log_row_t *log;
}log_node;

static inline struct log_row_t* node_to_log(struct klist_node *knode)
{
    return ((struct log_node*)knode)->log;