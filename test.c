#include <stdio.h>
#include <fcntl.h>


int main()
{
    char buf[4];

    FILE *fp = open("/sys/class/fw/rules/rules", "r+");

    fclose(fp);
    return 0;
}