#include "connection_table_list.h"

/*
    The get function of the klist. It will be useful for node-related memory allocation.
    This function is called each time a node is added to the list, and in our use-case every time klist_add_tail is called on a node, with node as klist_add_tail's parameter.

    Parameters:
    - n: The node that is being added to a list.
*/
void connection_table_get(struct klist_node* n)
{
    ((node*)n)->log = NULL;
    ((node*)n)->log = kmalloc(sizeof(connection_table_entry), GFP_KERNEL);
}