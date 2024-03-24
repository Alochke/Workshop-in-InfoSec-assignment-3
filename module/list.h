/*
    The list h and c files were made to eliminate code duplication.
    The list_destroy function of logs_list and connection_table_list look exactly the same.
    So here we store one that suitable for both.
*/

#ifndef LIST
#define LIST

void list_destroy(struct klist *list, struct klist_iter *iter)

#endif