#include <fcntl.h>
#include <unistd.h>


int main()
{
    fd idk = open("/sys/class/fw/rules/rules");

    write(idk, "idk", 1);
    
    return 0;
}