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
static int parseOptions(int argc, char **argv) {

    int i;
    for (i = 1; i < argc; i++) {
        int lastarg = i==argc-1;
        if (!strcmp(argv[i], "-p") && !lastarg) {
            config.pid = atoi(argv[++i]);

        } else if (!strcmp(argv[i],"-h") || !strcmp(argv[i], "--version")) {
            char *useage = getUseage();
            printf("useage:\n%s\n", useage);
            exit(0);
        } else if (!strcmp(argv[i],"-v") || !strcmp(argv[i], "--version")) {
            char *version = getVersion();
            printf("EDB %s\n", version);
            exit(0);
        } else {
            if (argv[i][0] == '-') {
                fprintf(stderr,
                        "Unrecognized option or bad number of args for: '%s'\n",
                        argv[i]);
                exit(1);
            } else {
                /* Likely the command name, stop here. */
                break;
            }
        }
    }


    return i;
}
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

 //   Debugger debugger("/home/kang/src/EDB/test_demo/dummy");
    std::cout << argv[1] << std::endl;
    std::string file_name(argv[1]);
    Debugger debugger(file_name);

    debugger.trace();
//    testTrace(debugger);
//    long address = 4198831;
//    std::cout << "Exam Memory before" << address << std::endl;
//    vector<long> v = debugger.examMemory(address, 8);
//    for (auto i : v)
//        printf("%lx ", i);
//    printf("\n");
    debugger.setBreakPointInLine(18);
    debugger.setBreakPointInLine(17);
    debugger.cancelBreakPointInLine(18);
    debugger.run();
    debugger.waitTracee();
    std::string reg = "RIP";
    printf("RIP %lx \n", debugger.examRegister(reg));
//    debugger.cancelBreakPointInLine(17);
//    std::cout << "Exam Memory after" << address << std::endl;
//    vector<long> vec = debugger.examMemory(address, 8);
//    for (auto i : vec)
//        printf("%lx ", i);
//    printf("\n");
    debugger.run();
    debugger.waitTracee();

}

//int main(int argc, char **argv) {
//    char *line;
//    char *prgname = argv[0];
//
//    parseOptions(argc, argv);
//
//    /* Set the completion callback. This will be called every time the
//     * user uses the <tab> key. */
//    linenoiseSetCompletionCallback(completion);
//    linenoiseSetHintsCallback(hints);
//
//    /* Load history from file. The history file is just a plain text file
//     * where entries are separated by newlines. */
//    linenoiseHistoryLoad("history.txt"); /* Load the history at startup */
//
//    /* Now this is the main loop of the typical linenoise-based application.
//     * The call to linenoise() will block as long as the user types something
//     * and presses enter.
//     *
//     * The typed string is returned as a malloc() allocated string by
//     * linenoise, so the user needs to free() it. */
//    while((line = linenoise("edb> ")) != NULL) {
//        /* Do something with the string. */
//        if (line[0] != '\0' && line[0] != '/') {
//            printf("echo: '%s'\n", line);
//            linenoiseHistoryAdd(line); /* Add to the history. */
//            linenoiseHistorySave("history.txt"); /* Save the history on disk. */
//        } else if (!strncmp(line,"/historylen",11)) {
//            /* The "/historylen" command will change the history len. */
//            int len = atoi(line+11);
//            linenoiseHistorySetMaxLen(len);
//        } else if (line[0] == '/') {
//            printf("Unreconized command: %s\n", line);
//        }
//        free(line);
//    }
//    return 0;
//}
