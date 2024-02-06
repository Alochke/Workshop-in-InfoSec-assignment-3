#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include "fw.h"


int main() {
    FILE* fptr = fopen("fd1", "w");
    int fd = open("/sys/class/fw/rules/rules", O_WRONLY);
    char buf[7 * sizeof(rule_t)];

    fread(buf, sizeof(rule_t), 7, fptr);
    write(fd, buf, sizeof(rule_t) * 7);

    close(fd);
    fclose(fptr);

    return 0;
}
