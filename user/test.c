#include <unistd.h> // For read system call.
#include <time.h> // For the timestamps.
#include <stdio.h>
int main(){
   time_t t = time(NULL);
   struct tm tm_instance;
   localtime_r(&t, &tm_instance);
   printf("%02d/%02d/%d %02d:%02d:%02d", tm_instance.tm_mday, tm_instance.tm_mon + 1, tm_instance.tm_year + 1900, tm_instance.tm_hour, tm_instance.tm_min, tm_instance.tm_sec);
}