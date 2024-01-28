#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include "fw.h"



int main()
{
    char buf1[sizeof(rule_t)];
    char buf2[sizeof(rule_t)];

    memset(buf1, 'a', sizeof(rule_t) - 1);
    buf1[sizeof(rule_t) - 1] = 0;
    memset(buf2, 'b', sizeof(rule_t) - 1);
    buf2[sizeof(rule_t) - 1] = 0;

    int file = fopen("/sys/class/fw/rules/rules", "r+");

    fputs(buf1, sizeof(rule_t), file);

    fgets(buf2, sizeof(rule_t), file);

    printf("%s\n", buf2);

    fclose(file);
    return 0;
}