//
// Created by kang on 19-2-15.
//

#include <cstdio>
#include <sys/ptrace.h>
#include <sys/types.h>
#include <unistd.h>
#include <asm/ptrace-abi.h>
//#include <sys/reg.h>
#include <sys/wait.h>
#include <iostream>
#include <map>
#include <string>
#include <cstring>
using namespace std;
int main(int argc, char **argv)
{
    pid_t pid = atoi(argv[1]);
    void *addr;

    if (-1 == ptrace(PTRACE_ATTACH, pid, NULL, NULL))
        perror("ptrace attach");
    int waitstatus;
    waitpid(pid, &waitstatus, 0);

    if (!strcmp(argv[2], "set") && !strcmp(argv[3], "mem")) {
        scanf("%p", &addr);
        long value = atoi(argv[4]);
        if (-1 == ptrace(PTRACE_POKETEXT, pid, addr, value))
            perror("ptrace set mem");
    }else if (!strcmp(argv[2], "get") && !strcmp(argv[3], "mem")) {
        scanf("%p", &addr);
        int mem_value = ptrace(PTRACE_PEEKTEXT, pid, addr, NULL);
        if (mem_value == -1)
            cout << "cannot accexx address : " << addr;
        printf("mem %s : %lx\n", argv[4], mem_value);
    }
    ptrace(PTRACE_CONT, pid, NULL, NULL);
}
