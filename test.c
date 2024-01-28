#include <stdio.h>


int main()
{
    char buf[4];

    FILE* file = fopen("/sys/class/fw/rules/rules", "r+");

    // fprintf(file, "idk");

    // fgets(buf, 4, file);

    // printf("%s", buf);

    // fclose(file);
    return 0;
}