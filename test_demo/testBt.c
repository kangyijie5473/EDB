#include <stdio.h>
void CCC()
{
    int c = 3;
    return;
}
void BBB()
{
    int b = 2;
    CCC();
    return;
}
void AAA()
{
    int a = 1;
    BBB();
    return;
}
int main()
{
    int e = 4;
    AAA();
    return 0;
}


