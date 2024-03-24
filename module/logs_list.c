#include "logs_list.h"

/*
    A get function of klist. It will be useful for node-related memory allocation.
    This function is called each time a node is added to the list, and in our use-case every time klist_add_tail is called on a node, with node as klist_add_tail's parameter.

    Parameters:
    - n: The node that is being added to a list.
*/
void logs_get(struct klist_node* n)
{
    ((node*)n)->content = NULL;
    ((node*)n)->content = kmalloc(sizeof(log_row_t), GFP_KERNEL);
}