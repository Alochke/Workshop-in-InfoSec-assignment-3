#include <stdio.h>


int main()
{
    char buf[4];

    FILE *fp = fopen("/sys/class/fw/rules/rules", "r+");

    fclose(fp);
    return 0;
}