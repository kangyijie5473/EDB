//
// Created by kang on 19-2-14.
//
#include <iostream>
#include <unistd.h>
int main()
{
    __asm__ __volatile__("movq $5473, %R15"); // test reg
    long value = 5473;                        // test mem
    sleep(100);
    //__asm__ __volatile__()
}

