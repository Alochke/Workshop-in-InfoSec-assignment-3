#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>


int main()
{
    char buf[4];

    int fp = open("/sys/class/fw/rules/rules", O_RDWR);

    close(fp);
    return 0;
}