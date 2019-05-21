#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>
int main()
{
    int rfd = open("/home/kang/src/Daily_Hole/ascii.c", O_RDONLY);
    int wfd = open("thin", O_WRONLY);
    char buffer[5] = {0};
    for (int i= 0; i < 512; i++) {
        read(rfd, buffer, 4);
        //printf(buffer);
    }
    close(rfd);
    char writeBuffer[128] = {'1'};
    write(wfd, writeBuffer, 128);

    close(wfd);
    return 0;
}