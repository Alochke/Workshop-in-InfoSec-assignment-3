#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include "fw.h"


int main() {
    int fd1 = open("idk", O_RDONLY);
    int fd2 = open("/sys/class/fw/rules/rules", O_WRONLY);
    char buf[7 * sizeof(rule_t)];
    int a = 0;


    while (a < sizeof(rule_t) * 7)
    {
        a += read(fd1, buf + a, 7 * sizeof(rule_t) - a);
    }
    write(fd2, buf, sizeof(rule_t) * 7);

    close(fd1);
    close(fd2);

    return 0;
}
