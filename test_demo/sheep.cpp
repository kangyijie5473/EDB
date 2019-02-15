//
// Created by kang on 19-2-14.
//
#include <iostream>
#include <unistd.h>
using namespace std;
int main()
{
    __asm__ __volatile__("movq $5473, %R15"); // test reg
    long value = 1024;                        // test mem
    cout << &value << endl;
    sleep(30);
    printf("%lx", value);
    //__asm__ __volatile__()
}

