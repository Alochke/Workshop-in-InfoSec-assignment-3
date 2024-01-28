#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main()
{
    char buf1[60];
    char buf2[60];

    memset(buf1, 'a', 59);
    buf1[59] = NULL;
    memset(buf2, 'b', 59);
    buf2[59] = NULL;

    FILE* file = fopen("/sys/class/fw/rules/rules", "r+");

    fwrite(buf1, 60, 1, file);
    fclose(file);

    fgets(buf2, 60, file);
    file = fopen("/sys/class/fw/rules/rules", "r+");

    printf("%s\n", buf2);

    fclose(file);
    return 0;
}