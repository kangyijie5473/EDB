//
// Created by kang on 19-2-14.
//
#include <stdio.h>
#include <unistd.h>
void func()
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
    printf("---write data to R15（5473）---\n");
    __asm__ __volatile__("movq $5473, %R15"); // 手动写数据到R15寄存器
    printf("---write data to R15 fin------\n");
    printf("---read data from R14--------\n");
    long v;
    __asm__ __volatile__("movq %%R14, %0" : "=r"(v));
    printf("---read data %ld from R14----\n", v);
    printf("---exam memmory-----------\n");
    long value = 1024;
    printf("address:%ld\n", &value);
    printf("---modify memory----------\n");
    printf("now value is %ld\n", value);
    printf("---memory fin-------------\n");
    printf("------backtrace and step--\n");
    func();
    printf("--backtrace and step fin--\n");
    printf("------sheep fin-----------\n");
    return 0;
}

