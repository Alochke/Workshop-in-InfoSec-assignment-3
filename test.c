#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>



// int main()
// {
//     char buf[4];

//     FILE* file = fopen("/sys/class/fw/rules/rules", "r+");

//     fprintf(file, "idk");

//     // fgets(buf, 4, file);

//     // printf("%s", buf);

//     // fclose(file);
//     return 0;
// }

int main()
{
    char buf[4];

    int file = open("/sys/class/fw/rules/rules", "r+");

    write(file, "idk", 1);

    // fgets(buf, 4, file);

    // printf("%s", buf);

    close(file);
    return 0;
}