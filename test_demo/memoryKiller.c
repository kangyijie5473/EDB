#include <stdlib.h>
#include <stdio.h>
int main()
{
    for (int i = 0; i < 100; i++) {
        char *p = (char *)malloc(4096);
        for(int j = 0; j < 4096; j++)
            p[j] = '\0';
    }
    printf("fisrt\n");
    for (int i = 0; i < 100; i++) {
        char *p = (char *)malloc(4096);
        for(int j = 0; j < 4096; j++)
            p[j] = '\0';
    }
    printf("second\n");
    for (int i = 0; i < 100; i++) {
        char *p = (char *)malloc(4096);
        for(int j = 0; j < 4096; j++)
            p[j] = '\0';
    }
    return 0;
}
