#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    char idk[3];

    idk[0] = ' ';
    idk[1] = '\0';
    idk[2] = 'a';

    while (1)
    {
        char* b = strtok(idk, " ");
        printf("%p", b);
    }
    
    

    return 0;
}
