#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include "fw.h"


int main() {
    FILE* fptr = fopen("idk", "r");
    int fd = open("/sys/class/fw/rules/rules", O_WRONLY);
    char buf[7 * sizeof(rule_t)];

    int a = fread(buf, sizeof(rule_t), 7, fptr);
    printf("%p\n", fptr);
    write(fd, buf, sizeof(rule_t) * 7);

    close(fd);
    fclose(fptr);

    return 0;
}
