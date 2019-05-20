#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
int main()
{
    chdir("/home/kang/src/EDB/test_demo");
    int fd1 = open("threadPool.c", O_RDONLY);
    int fd2 = open("moveCwd.c", O_RDONLY);
    int fd3 = open("memoryKiller.c", O_RDONLY);
    printf("finish open fd\n");
    close(fd1);
    close(fd2);
    close(fd3);
    return 0;
}
