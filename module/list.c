#include "list.h"

/*
    The get method of the klist, will be useful for log_node-related memory management.
*/
void put(struct klist_node* node)
{
    ((log_node*)node)->log = NULL;
    ((log_node*)node)->log = kmalloc(GFP_KERNEL, sizeof(log_row_t));
}

/*
    The put method of the klist, will be useful for its log_node-related memory management.
*/
void get(struct klist_node* node)
{
    if (((log_node*)node)->log != NULL)
    {
        kfree(((log_node*)node)->log);
    }
}

/*
    Destroies list, by using iter.
    
    Parameters:
    - list: A pointer to the list we're destroying, it will be log_list from list.c in practice.
    - iter: A pointer to the iter we'll use to iterate over list, it will be klist_iter from list.c in parctice.
*/
void list_destroy(struct klist *list, struct klist_iter *iter)
{
    for (klist_iter_init(list, iter); &klist_next(iter)->n_node != &list->k_list; klist_del(iter->i_cur));
    klist_iter_exit(iter);
}