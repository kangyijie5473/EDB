//
// Created by kang on 19-2-14.
//
#include <stdio.h>
#include <sys/ptrace.h>
#include <sys/types.h>
#include <unistd.h>
#include <asm/ptrace-abi.h>
#include <sys/reg.h>
#include <sys/wait.h>
int main()
{
    pid_t pid = atoi(argv[1]);
    if (!strcmp(argv[2], "showall")) {

    }
    wait(NULL);
    long ori_eax;
    ori_eax = ptrace(PTRACE_PEEKUSER, pid, 4 * ORIG_RAX, NULL);
    printf("the child process using system call NO.%d", ori_eax);
    scanf("%d",&pid);
}

