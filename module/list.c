#include "list.h"

/*
    Resets list, by using iter.
    This doesn't free list, for reusability reasons.
            
    Parameters:
    - list: A pointer to the list we're destroying.
    - iter: A pointer to the iter we'll use to iterate over the list.
*/
void list_destroy(struct klist *list, struct klist_iter *iter)
{   
    for (klist_iter_init(list, iter); klist_next(iter) != NULL; klist_del(iter->i_cur));
    klist_iter_exit(iter);
}

/*
    The put function of the klist. It will be useful for node-related memory deallocation.
    This function is called each time a node is removed from the list, and in our use-case every time klist_del is called on a node, with node as klist_del's parameter.

    Parameters:
    - n: The node that is being removed from a list.
*/
void list_put(struct klist_node* n)
{
    if (likely(((node*)n)->content != NULL))
    {
        kfree(((node*)n)->content);
    }
}