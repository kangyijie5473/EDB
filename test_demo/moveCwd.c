#include <stdio.h>
#include <unistd.h>
int main()
{
    printf("start change current work direntory\n");
    chdir("/usr/include/rdma");
    printf("finish change current work direntory\n");
}
