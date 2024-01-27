#include <fcntl.h>
#include <unistd.h>


int main()
{
    int idk = open("/sys/class/fw/rules/rules", O_RDWR);

    write(idk, "idk", 1);
    
    return 0;
}