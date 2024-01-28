#include <stdio.h>


int main()
{
    char buf[4];

    FILE *fp = fopen("/sys/class/fw/rules/rules", "r+");
    
    fprintf(fp, "lol");

    fgets(buf, 4, fp);

    printf("%s", buf);

    fclose(fp);
    return 0;
}