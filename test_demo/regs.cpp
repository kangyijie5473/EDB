//
// Created by kang on 19-2-14.
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
map<int, string> x64_regs {
        {0, "R15"},
        {1, "R14"},
        {2, "R13"},
        {3, "R12"},
        {4, "RBP"},
        {5, "RBX"},
        {6, "R11"},
        {7, "R10"},
        {8, "R9"},
        {9, "R8"},
        {10, "RAX"},
        {11, "RCX"},
        {12, "RDX"},
        {13, "RSI"},
        {14, "RDI"},
        {15, "ORIG_RAX"},
        {16, "RIP"},
        {17, "CS"},
        {18, "EFLAGS"},
        {19, "RSP"},
        {20, "SS"},
        {21, "FS_BASE"},
        {22, "GS_BASE"},
        {23, "DS"},
        {24, "ES"},
        {25, "FS"},
        {26, "GS"}
};
map<string, int> x64_regs_num {
        {"R15", 0},
        {"R14", 1},
        {"R13", 2},
        {"R12", 3},
        {"RBP", 4},
        {"RBX", 5},
        {"R11", 6},
        {"R10", 7},
        {"R9", 8},
        {"R8", 9},
        {"RAX", 10},
        {"RCX", 11},
        {"RDX", 12},
        {"RSI", 13},
        {"RDI", 15},
        {"ORIG_RAX", 15},
        {"RIP", 16},
        {"CS", 17},
        {"EFLAGS", 18},
        {"RSP", 19},
        {"SS", 20},
        {"FS_BASE", 21},
        {"GS_BASE", 22},
        {"DS", 23},
        {"ES", 24},
        {"FS", 25},
        {"GS", 26}
};
int main(int argc, char **argv)
{
    pid_t pid = atoi(argv[1]);
    ptrace(PTRACE_ATTACH, pid, NULL, NULL);
    int waitstatus;
    waitpid(pid, &waitstatus, 0);
    long reg_value;
    if (!strcmp(argv[2], "showall")) {
        for (auto i : x64_regs) {

            reg_value = ptrace(PTRACE_PEEKUSER, pid, 8 * i.first, NULL);
            if (reg_value == -1)
                perror("ptrace");
            printf("%s : 0x%lx %lld\n", i.second.c_str(), reg_value, reg_value);
            //cout << i.second << " : " << reg_value << endl;
        }
        ptrace(PTRACE_CONT, pid, NULL, NULL);
    } else if(!strcmp(argv[2], "set") && !strcmp(argv[3], "reg")) {
        long v = atoi(argv[5]);
        string obj_reg(argv[4]);
        ptrace(PTRACE_POKEUSER, pid, 8 * x64_regs_num[obj_reg], v);
        reg_value = ptrace(PTRACE_PEEKUSER, pid, 8 * x64_regs_num[obj_reg], NULL);
        printf("%s : 0x%lx %lld\n", argv[4], reg_value, reg_value);

    }
//    wait(NULL);
//    long ori_eax;
//    ori_eax = ptrace(PTRACE_PEEKUSER, pid, 8 * x64::ORIG_RAX, NULL);
//    printf("the child process using system call NO.%d", ori_eax);
//    scanf("%d",&pid);
}

