
#include <unistd.h>
#include <fcntl.h>
#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <iostream>
#include <cstdlib>

#include <linenoise.h>
#include <dwarf++.hh>
#include <elf.h>
#include <elf++.hh>

#include "common.h"
#include "Debugger.h"
#include "Displayer.h"
#include "Watcher.h"

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


static char *getVersion()
{
    return const_cast<char *>("0.1");
}
static char *getUseage()
{
    return const_cast<char *>("EDB -p pid\nEDB\nEDB a.out");
}
static struct Config config;
static void parseOptions(int argc, char **argv) {

    int i;
    for (i = 1; i < argc; i++) {
        int lastarg = i == argc-1;
        if (!strcmp(argv[i], "-p") && !lastarg) {
            config.pid = atoi(argv[++i]);
            config.start_flag = START_PID;
            return;
        } else if (!strcmp(argv[i],"-h") || !strcmp(argv[i], "--version")) {
            char *useage = getUseage();
            printf("useage:\n%s\n", useage);
            exit(0);
        } else if (!strcmp(argv[i],"-v") || !strcmp(argv[i], "--version")) {
            char *version = getVersion();
            printf("EDB %s\n", version);
            exit(0);
        } else if (argv[i][0] == '-') {
            fprintf(stderr, "Unrecognized option or bad number of args for: '%s'\n", argv[i]);
            exit(1);
        } else if (argv[i][0] != '-') {
            config.file_name = argv[i];
            config.start_flag = START_FILENAME;
            return;
        }
    }

    config.start_flag = STRAT_ALONE;
}
std::vector<std::string> pre_cmd;
int handleCommand(Debugger &debugger, const char *input_line, int status)
{
    std::string s = " ";
    std::vector<string> cmd;
    std::string line(input_line);
    split(line, s, cmd);

    //if input_line just has *\n*, exec pre command
    if(cmd.size() == 0) {
        if (pre_cmd.size() == 0) {
            printf("\n");
            return status;
        } else {
            cmd = pre_cmd;
        }

    }

    if (cmd[0] == "step" || cmd[0] == "s") {
        status = debugger.stepInto();
    } else if (cmd[0] == "finish")
        status = debugger.stepOut();
    else if (cmd[0] == "next" || cmd[0] == "n")
        debugger.stepOver();
    else if(cmd.size() == 2 && (cmd[0] == "b" || cmd[0] == "break")) {
        int line_num = atoi(cmd[1].c_str());
        if (C_OK == debugger.setBreakPointInLine(line_num))
            printf("**set breakpoint in Line:%d**\n", line_num);
    } else if(cmd[0] == "c" || cmd[0] == "continue") {
        debugger.run();
        status = debugger.waitTracee();
    } else if (cmd.size() == 2 && (cmd[0] == "info"|| cmd[0] == "i") && (cmd[1] == "breakpoints" || cmd[1] == "b"))
        debugger.printBreakPointList();
    else if (cmd.size() == 3 && (cmd[0] == "i" || cmd[0] == "info") && (cmd[1] == "registers" || cmd[1] == "r"))
        printf("**Register %s:%ld**\n", cmd[2].c_str(), debugger.examRegister(cmd[2]));
    else if (cmd.size() == 2 && (cmd[0] == "i" || cmd[0] == "info") && (cmd[1] == "registers" || cmd[1] == "r")) {
        for (auto i : x64_regs_num) {
            printf("**Register %s:%ld**\n", i.first.c_str(), debugger.examRegister(i.first.c_str()));
        }
    } else if (cmd[0] == "print" && cmd[1] == "memory") {
        long address = atol(cmd[2].c_str());
        printf("**Memory address :%ld value:%ld**\n", address, debugger.examMemory(address));
    } else if (cmd[0] == "backtrace" || cmd[0] == "bt") {
        debugger.backtrace();
    } else if (cmd[0] == "set" && cmd[1] == "memory") {
        long address = atol(cmd[2].c_str());
        int value = atoi(cmd[3].c_str());
        debugger.modifyMemory(address, value);
    } else if(cmd.size() == 2 && (cmd[0] == "print" || cmd[0] == "p") ) {
        debugger.examVariable(cmd[1]);
    } else if(cmd[0] == "test") {
        printf("test\n");
    } else
        printf("**wrong command**\n");
    // save cmd to next input *enter* only
    pre_cmd = cmd;
    return status;
}

int main(int argc, char **argv)
{
    //init Debugger and set line-edit hints
    parseOptions(argc, argv);
    char *input_line;
    linenoiseSetCompletionCallback(completion);
    linenoiseSetHintsCallback(hints);
    Debugger debugger(config);

    //start trace
    int status = debugger.trace();
    printf("****start trace****\n");

    //set default breakpoint in main function
    if (C_OK == debugger.setBreakPointInStart())
        printf("**set tempory break in main function**\n");
    else
        cout << "**can't set tempory break in main function**\n" << endl;


    // main loop
    while (WIFSTOPPED(status) && ((input_line = linenoise("(edb) ")) != NULL)) {
        status = handleCommand(debugger, input_line, status);
        free(input_line);
        if (WIFEXITED(status))
            break;
    }

    //detach tracee
    cout << "****trace is over****" << endl;
    debugger.detach();

    return 0;
}
