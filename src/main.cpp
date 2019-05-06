#include <iostream>
#include <linenoise.h>
#include <dwarf++.hh>
#include <elf.h>
#include <elf++.hh>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <cstdlib>
#include "Debugger.h"

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

void completion(const char *buf, linenoiseCompletions *lc) {
    if (buf[0] == 'h') {
        linenoiseAddCompletion(lc,"hello");
        linenoiseAddCompletion(lc,"hello there");
    }
}

char *hints(const char *buf, int *color, int *bold) {
    if (!strcasecmp(buf,"hello")) {
        *color = 35;
        *bold = 0;
        return " World";
    }
    return NULL;
}
static char *getVersion()
{
    return const_cast<char *>("0.1");
}
static char *getUseage()
{
    return const_cast<char *>("EDB -p pid\nEDB\nEDB a.out");
}
static struct config {
   int fd;          //fd
   char *file_name;
   char *file_path;
   int pid;

} config;
//static int parseOptions(int argc, char **argv) {
//
//    int i;
//    for (i = 1; i < argc; i++) {
//        int lastarg = i==argc-1;
//        if (!strcmp(argv[i], "-p") && !lastarg) {
//            config.pid = atoi(argv[++i]);
//
//        } else if (!strcmp(argv[i],"-h") || !strcmp(argv[i], "--version")) {
//            char *useage = getUseage();
//            printf("useage:\n%s\n", useage);
//            exit(0);
//        } else if (!strcmp(argv[i],"-v") || !strcmp(argv[i], "--version")) {
//            char *version = getVersion();
//            printf("EDB %s\n", version);
//            exit(0);
//        } else {
//            if (argv[i][0] == '-') {
//                fprintf(stderr,
//                        "Unrecognized option or bad number of args for: '%s'\n",
//                        argv[i]);
//                exit(1);
//            } else {
//                /* Likely the command name, stop here. */
//                break;
//            }
//        }
//    }
//
//
//    return i;
//}
//static void testTrace(Debugger &debugger)
//{
//    pid_t pid = fork();
//    if (pid == 0) {
//        long ret;
//        if ((ret = ptrace(PTRACE_TRACEME, 0, NULL, NULL)) == -1)
//            perror("trace me");
//        else
//            std::cout << "traeceme ret" << ret << std::endl;
//        execl(debugger.file_name.c_str(), debugger.file_name.c_str(), NULL);
//    } else {
//        debugger.pid = pid;
//
//        std::cout << "father pid " << debugger.pid << "filename " << debugger.file_name.c_str() << std::endl;
//
////        sleep(20);
//        int status;
//        waitpid(debugger.pid, &status, 0);
//        std::cout << "wait status " << status << std::endl;
//    }
//}

int main(int argc, char **argv)
{



    std::cout << argv[1] << std::endl;
    std::string file_name(argv[1]);
//    int pid = atoi(argv[1]);
    Debugger debugger(file_name);

    int status = debugger.trace();
    cout << "****start trace****" << endl;

    if (C_OK == debugger.setBreakPointInFunc("main")) //set bp in main func
        cout << "**set tempory break in main function**" << endl;
    else
        cout << "can't set tempory break in main function" << endl;
    debugger.run();
    status = debugger.waitTracee();
    debugger.cancelAllBreakPoint();
    long rip = debugger.examRegister("RIP");
    debugger.modifyRegister("RIP", rip - 1);
    while (WIFSTOPPED(status)) {
//
//        printf("RIP->%lx\n", debugger.examRegister("RIP"));
        debugger.printSourceLine();
        string line;
        string s = " ";
        vector<string> cmd;
        cout << "edb>";
        getline(cin, line);
        split(line, s, cmd);


        if (cmd[0] == "step") {
            status = debugger.stepInto();

        } else if (cmd[0] == "finish")
            status = debugger.stepOut();
        else if (cmd[0] == "next")
            debugger.run();
        else if(cmd[0] == "set" && cmd[1] == "breakpoint") {
            int line_num = atoi(cmd[2].c_str());
            if (C_OK == debugger.setBreakPointInLine(line_num))
                printf("**set breakpoint in Line:%d**\n", line_num);
        } else if(cmd[0] == "run") {
            debugger.run();
            status = debugger.waitTracee();
        } else if (cmd[0] == "print" && cmd[1] == "breakpoint")
            debugger.printBreakPointList();
        else if (cmd[0] == "print" && cmd[1] == "register")
            printf("**Register %s:%ld**\n", cmd[2].c_str(), debugger.examRegister(cmd[2]));
        else if (cmd[0] == "print" && cmd[1] == "memory") {
            long address = atol(cmd[2].c_str());
            printf("**Memory address :%ld value:%ld**\n", address, debugger.examMemory(address));
        } else if (cmd[0] == "backtrace") {
            debugger.backtrace();
        } else if (cmd[0] == "set" && cmd[1] == "memory") {
            long address = atol(cmd[2].c_str());
            int value = atoi(cmd[3].c_str());
            debugger.modifyMemory(address, value);
        } else if (cmd[0] == "la") {
            debugger.printLineAddress();
        } else if(cmd[0] == "print" && cmd[1] == "var") {
            debugger.examVariable(cmd[2]);
        } else if(cmd[0] == "test") {
            ;
        } else
            cout << "wrong command" << endl;

        if (WIFEXITED(status))
            break;
    }
    cout << "****trace is over****" << endl;
    debugger.detach();


    return 0;
}
