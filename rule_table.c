#include "rule_table.h"

int main()
{
    printk("%d\n", sizeof(sizeof(rule_t) * MAX_RULES));
    printk("%ul\n", PAGE_SIZE);
}