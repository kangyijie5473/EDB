#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
int add(int a, int b)
{
    printf("-----LINE4------\n");
    printf("-----LINE5------\n");
    printf("-----LINE6------\n");
    return a + b;
}
int main(int argc, char **argv)
{
	//printf("--------%s-----\n", argv[0]);
	int a = 1;
	//int *p = NULL;
    printf("-----LINE14------\n");
    printf("-----LINE15------\n");
    int b = 2;
	int c = 3;
	b = add(b, c);
    printf("-----LINE19------\n");
    int d = 4;
    sleep(3600);
    return 0;
}
