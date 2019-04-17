//
// Created by kang on 19-3-6.
//
#include "elf/elf++.hh"
#include "dwarf/dwarf++.hh"

#include <errno.h>
#include <fcntl.h>
#include <inttypes.h>
#include <stdio.h>
#include <sys/ptrace.h>
#include <sys/types.h>
#include <unistd.h>
#include <asm/ptrace-abi.h>
#include <sys/reg.h>
#include <sys/wait.h>
#include <string>
#include <iostream>
#include <vector>
using namespace std;
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
void split(std::string& s, std::string& c,std::vector< std::string >& v)
{
    std::string::size_type pos1, pos2;
    size_t len = s.length();
    pos2 = s.find(c);
    pos1 = 0;
    while(std::string::npos != pos2)
    {
        v.emplace_back(s.substr(pos1, pos2-pos1));

        pos1 = pos2 + c.size();
        pos2 = s.find(c, pos1);
    }
    if(pos1 != len)
        v.emplace_back(s.substr(pos1));
}
void getLineMap(const dwarf::line_table &lt, map<int, long> &line_map)
{
    for (auto &line : lt) {
        line_map.insert(make_pair(line.line, line.address));
    }
    return;
}
char old_ins;
long buf;
int line_num;
int waitstatus;
int main(int argc, char const *argv[])
{
    int fd = open("dummy", O_RDONLY);

    map<int, long> line_map;
    elf::elf ef(elf::create_mmap_loader(fd));
    dwarf::dwarf dw(dwarf::elf::create_loader(ef));
    for (auto cu : dw.compilation_units()) {
        getLineMap(cu.get_line_table(), line_map);
    }

    pid_t pid;
    pid = fork();
    if (pid == 0) {
        ptrace(PTRACE_TRACEME, 0, NULL, NULL);
        execl("./dummy", "dummy", NULL);

    } else {
        printf("trace pid%d\n", pid);
        wait(NULL);

        while (true) {
            string cmd;
            cout << "input:";
            getline(cin, cmd);
            vector<string> cmd_sp;
            string flag = " ";
            split(cmd, flag, cmd_sp);

            if (cmd_sp.size() == 1) {
                if (cmd_sp[0] == "run") {
                    ptrace(PTRACE_CONT, pid, NULL, NULL);
                    waitpid(pid, &waitstatus, 0);
                    cout << "---stop--";
                    char *p = (char *)&buf;
                    *p = old_ins;
                    printf("buf:%lx\n",buf);
                    long address = line_map[line_num];
                    int s;
                    cin >> s;
                    if (-1 == ptrace(PTRACE_POKETEXT, pid, address, buf))
                        perror("ptrace");

                    long rip = ptrace(PTRACE_PEEKUSER, pid, 8 * x64_regs_num["RIP"], NULL);
                    printf("rip %lx\n", rip);
                    rip--;
                    ptrace(PTRACE_POKEUSER, pid , 8 * x64_regs_num["RIP"], rip);
                    ptrace(PTRACE_CONT, pid, NULL, NULL);
                    int status;
                    waitpid(pid, &status, 0);

                    cout << "fin status is"<< status << endl;
                    ptrace(PTRACE_DETACH, pid, NULL, NULL);
                    waitpid(pid, &status, 0);
                    cout << "detach status is" << status << endl;


                    int a;
                    cin >>  a;
                    return 0;


                } else if (cmd_sp[0] == "step") {

                } else if (cmd_sp[0] == "next")
                    break;
            }
            if (cmd_sp.size() == 3) {
                if (cmd_sp[0] == "set" && cmd_sp[1] == "bp") {
                    line_num = atoi(cmd_sp[2].c_str());
                    long address = line_map[line_num];
                    const char magic_num = 0xcc;
                    buf = ptrace(PTRACE_PEEKTEXT, pid, address, NULL);
                    printf("buf: %lx\n",buf);
                    char *p = (char *)&buf;
                    old_ins = *p;
                    *p = magic_num;
                    ptrace(PTRACE_POKETEXT, pid, address, buf);
                    cout << "set bp in Line:"<<  cmd_sp[2];
                    printf("address %lx\n", address);
                 } else if (cmd_sp[0] == "cancel" && cmd_sp[1] == "bp") {
                    cout << "cancel bp" << endl;
                }
            }

        }
    }
    return 0;
}





