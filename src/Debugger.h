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
#include <memory>
#include <algorithm>
#include <sys/user.h>
#include <libunwind.h>
#include <libunwind-x86_64.h>
#include <libunwind-ptrace.h>
#include <set>
#include <fstream>


class Debugger;

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
enum class reg {
    rax, rbx, rcx, rdx,
    rdi, rsi, rbp, rsp,
    r8,  r9,  r10, r11,
    r12, r13, r14, r15,
    rip, rflags,    cs,
    orig_rax, fs_base,
    gs_base,
    fs, gs, ss, ds, es
};

static constexpr std::size_t n_registers = 27;

struct reg_descriptor {
    reg r;
    int dwarf_r;
    std::string name;
};
static const std::array<reg_descriptor, n_registers> g_register_descriptors {{
     { reg::r15, 15, "r15" },
     { reg::r14, 14, "r14" },
     { reg::r13, 13, "r13" },
     { reg::r12, 12, "r12" },
     { reg::rbp, 6, "rbp" },
     { reg::rbx, 3, "rbx" },
     { reg::r11, 11, "r11" },
     { reg::r10, 10, "r10" },
     { reg::r9, 9, "r9" },
     { reg::r8, 8, "r8" },
     { reg::rax, 0, "rax" },
     { reg::rcx, 2, "rcx" },
     { reg::rdx, 1, "rdx" },
     { reg::rsi, 4, "rsi" },
     { reg::rdi, 5, "rdi" },
     { reg::orig_rax, -1, "orig_rax" },
     { reg::rip, -1, "rip" },
     { reg::cs, 51, "cs" },
     { reg::rflags, 49, "eflags" },
     { reg::rsp, 7, "rsp" },
     { reg::ss, 52, "ss" },
     { reg::fs_base, 58, "fs_base" },
     { reg::gs_base, 59, "gs_base" },
     { reg::ds, 53, "ds" },
     { reg::es, 50, "es" },
     { reg::fs, 54, "fs" },
     { reg::gs, 55, "gs" },
    }};
const int START_PID = 1;
const int START_FILENAME = 2;
const int STRAT_ALONE = 3;
struct Config {
    char *file_name;
    int start_flag;
    int pid;

} ;
class Debugger {
public:
    Debugger(int pid);
    Debugger(const std::string &file_name);
    Debugger(const struct Config &c);
    Debugger() = default;

    long examVariable(const std::string &name);
    long modifyVariable(const std::string &name, long value);
    std::vector<long> examMemory(long address, long size);

    long examMemory(const std::string &address);
    long modifyMemory(const std::string &address, const std::string &value);
    long setBreakPointInLine(int line_num);
    long cancelBreakPointInLine(int line_num);
    long setBreakPointInFunc(std::string func_name);
    long cancelBreakPointInFunc(std::string func_name);
    long setBreakPointInStart();
    void backtrace();
    long examRegister(const std::string &reg);
    long modifyRegister(std::string reg, long value);
    int stepInto();
    int stepOver();
    int stepOut();
    void run();
    int trace();
    int waitTracee();
    void detach();
    long attach(int pid);
    void cancelAllBreakPoint();
    void printSourceLine();
    //for test
    void printLineAddress();
    dwarf::die getFunctionDie();
    //test
    bool isDwarfFile();
    void printBreakPointList();
    pid_t getTracePid();

private:
    long init();
    long getFuncAddress(std::string func_name);
    void checkBreakPoint();
    long modifyMemory(long address, long value);
    long examMemory(long address);

    pid_t pid;
    std::string file_name;
    int fd;
    std::map<int, long> line_address;
    elf::elf elf_file;
    dwarf::dwarf dwarf_file;
    std::vector<std::pair<int, char>> breakpoint_list;
    std::set<long> line_address_set;
    std::map<long, int> address_line;
    std::vector<std::string> code_line;
};


#endif //SRC_DEBUGGER_H
