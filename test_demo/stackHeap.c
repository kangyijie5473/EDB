#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int main()
{
    printf("eat memory start\n");
    char *h = (char *)malloc(1024);
    long s = 1000;
    printf("---finish---\n");
    return 0;
}

