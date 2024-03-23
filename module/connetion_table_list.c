#include "logs_list.h"

/*
    Resets list, by using iter.
    This doesn't free list, for reusability reasons.
            
    Parameters:
    - list: A pointer to the list we're destroying, it will be connection_table from connection_table.c in practice.
    - iter: A pointer to the iter we'll use to iterate over the list, it will be iter from logs.c in practice.
*/
void list_destroy(struct klist *list, struct klist_iter *iter)
{   
    for (klist_iter_init(list, iter); klist_next(iter) != NULL; klist_del(iter->i_cur));
    klist_iter_exit(iter);
}