#include <stdio.h>
#include <fcntl.h>


int main()
{
    char buf[4];

    FILE *fp = open("/sys/class/fw/rules/rules", O_RDWR);

    fclose(fp);
    return 0;
}