//
// Created by kang on 19-4-15.
//

#ifndef SRC_DEBUGGER_H
#define SRC_DEBUGGER_H

#include <string>
#include <vector>
#include <sys/ptrace.h>
#include <map>
#include <elf++.hh>
#include <dwarf++.hh>
#include <unistd.h>
#include <fcntl.h>
#include <sstream>
#include <sys/wait.h>
#include <iostream>

const int C_OK = 0;
const int C_ERR = -1;
const char INT_ins = (char)0xcc;
const int NOT_ATTACH = -1;
class Debugger;
static void testTrace(Debugger &debugger);

static std::map<int, std::string> x64_regs {
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

static std::map<std::string, int> x64_regs_num {
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
class Debugger {
public:
    Debugger(int pid);
    Debugger(const std::string &file_name);
    Debugger() = default;

    long examVariable(std::string name);
    long modifyVariable(std::string name, long value);
    std::vector<long> examMemory(long address, long size);
    long modifyMemory(long address, std::string value);
    long setBreakPointInLine(long line_num);
    long cancelBreakPointInLine(long line_num);
    void backtrace();
    long examRegister(const std::string &reg);
    long modifyRegister(std::string reg, long value);
    void singleStep();
    void run();
    long trace();
    int waitTracee();



private:
    long init();
    pid_t pid;
    std::string file_name;
    int fd;
    std::map<int, long> line_address;
    elf::elf elf_file;
    dwarf::dwarf dwarf_file;
    std::vector<std::pair<long, char>> breakpoint_list;


};


#endif //SRC_DEBUGGER_H
