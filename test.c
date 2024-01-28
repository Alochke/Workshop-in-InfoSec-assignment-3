#include <stdio.h>


int main()
{
    char buf[4];

    FILE *fp = fopen("/sys/class/fw/rules/rules", "r+");
    
    fprintf(fp, &"lol");


    fclose(fp);
    return 0;
}