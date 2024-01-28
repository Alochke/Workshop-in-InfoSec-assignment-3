#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>



int main()
{
    char buf[4];

    int file = open("/sys/class/fw/rules/rules", O_RDWR);

    write(file, "idk", 1);

    // fgets(buf, 4, file);

    printf("%s", buf);

    close(file);
    return 0;
}