//
// Created by kang on 19-3-6.
//

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
int main(int argc, char const *argv[])
{
    pid_t pid;
    pid = fork();
    if (pid == 0) {
        ptrace(PTRACE_TRACEME, 0, NULL, NULL);
        execl("./sheep", "sheep", NULL);

    } else {
        wait(NULL);
        while (true) {
            string cmd;
            getline(cin, cmd);
            vector<string> cmd_sp;
            string flag = " ";
            split(cmd, flag, cmd_sp);
//        for (auto &i : cmd_sp)
//            cout << i << endl;
            if (cmd_sp.size() == 1) {
                if (cmd_sp[0] == "run") {
                    ptrace(PTRACE_CONT, pid, NULL, NULL);
                    break;
                    //waitpid(pid, &waitstatus, 0);
                } else if (cmd_sp[0] == "step") {

                } else if (cmd_sp[0] == "next")
                    break;
            }
            if (cmd_sp.size() == 3) {
                if (cmd_sp[0] == "set" && cmd_sp[1] == "bp") {
                    cout << "set bp" << endl;
                } else if (cmd_sp[0] == "cancel" && cmd_sp[1] == "bp") {
                    cout << "cancel bp" << endl;
                }
            }

        }
    }
    return 0;
}

int main(int argc, char const *argv[])
{
    pid_t pid = atoi(argv[1]);

    if (-1 == ptrace(PTRACE_ATTACH, pid, NULL, NULL))
        perror("ptrace attach");
    int waitstatus;
    waitpid(pid, &waitstatus, 0);






//    if (!strcmp(argv[2], "set") && !strcmp(argv[3], "mem")) {
//        scanf("%p", &addr);
//        long value = atoi(argv[4]);
//        if (-1 == ptrace(PTRACE_POKETEXT, pid, addr, value))
//            perror("ptrace set mem");
//    }else if (!strcmp(argv[2], "get") && !strcmp(argv[3], "mem")) {
//        scanf("%p", &addr);
//        int mem_value = ptrace(PTRACE_PEEKTEXT, pid, addr, NULL);
//        if (mem_value == -1)
//            cout << "cannot accexx address : " << addr;
//        printf("mem %s : %lx\n", argv[4], mem_value);
//    }
    return 0;
}