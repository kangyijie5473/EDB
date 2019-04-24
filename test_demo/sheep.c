//
// Created by kang on 19-2-14.
//
#include <stdio.h>
#include <unistd.h>
int add(int a, int b)
{
    return a + b;
}
void func(int a)
{
    printf("------step 1------\n");
    printf("------step 2------\n");
    printf("------step 3------\n");
    printf("------step 4------\n");
    printf("------step 5------\n");
    return;
}

int main()
{
    printf("---手动写数据到R15（5473）---\n");
    __asm__ __volatile__("movq $5473, %R15"); // 手动写数据到R15寄存器
    printf("---写完--------------------\n");
    printf("---exam memmory-----------\n");
    long value = 1024;
    printf("address:%ld\n", &value);
    printf("---modify memory----------\n");
    printf("now value is %ld\n", value);
    printf("---memory fin-------------\n");
    printf("------backtrace and step--\n");
    func(3);
    printf("--backtrace and step fin--\n");
    printf("------sheep fin-----------\n");
    return 0;
}

