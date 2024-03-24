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