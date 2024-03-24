#include "logs_list.h"

/*
    The put function of the klist. It will be useful for log_node-related memory deallocation.
    This function is called each time a node is removed from the list, and in our use-case every time klist_del is called on a node, with node as klist_del's parameter.

    Parameters:
    - node: The node that is being removed from a list.
*/
void list_put(struct klist_node* node)
{
    if (likely(((log_node*)node)->log != NULL))
    {
        kfree(((log_node*)node)->log);
    }
}

/*
    The get function of the klist. It will be useful for log_node-related memory allocation.
    This function is called each time a node is added to the list, and in our use-case every time klist_add_tail is called on a node, with node as klist_add_tail's parameter.

    Parameters:
    - node: The node that is being added to a list.
*/
void list_get(struct klist_node* node)
{
    ((log_node*)node)->log = NULL;
    ((log_node*)node)->log = kmalloc(sizeof(log_row_t), GFP_KERNEL);
}