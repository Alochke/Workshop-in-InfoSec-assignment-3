#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>


int main()
{
    char buf[4];

    int fd = open("/sys/class/fw/rules/rules", O_RDWR);

    write(fd, "lol", 4)

    close(fp);
    return 0;
}