#include <stdio.h>
#include <stdlib.h>
int add(int a, int b)
{
	return a + b;
}

int sub(int a, int b)
{
	return a - b;
}
int main(int argc, char **argv)
{
	printf("--------%s-----\n", argv[0]);
	int a = 1;
	int *p = &a;
    printf("-----LINE17------\n");
    printf("-----Line18------\n");
    int b = 2;
	int c = 3;
	int d = 4;
	a = add(b, c);
	a = sub(d, c);
    return 0;
}
