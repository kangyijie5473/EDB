//
// Created by kang on 19-4-27.
//

#include <map>
#include "Displayer.h"
void completion(const char *buf, linenoiseCompletions *lc) {
    if (buf[0] == 'b') {
        linenoiseAddCompletion(lc,"break");
        linenoiseAddCompletion(lc, "backtrace");
    }
    if (buf[0] == 'i') {
        linenoiseAddCompletion(lc, "info");
    }
    if (buf[0] == 's') {
        linenoiseAddCompletion(lc, "step");
    }
    if (buf[0] == 'p') {
        linenoiseAddCompletion(lc, "print");
    }
    if (buf[0] == 'c') {
        linenoiseAddCompletion(lc, "continue");
    }
    if (buf[0] == 'f') {
        linenoiseAddCompletion(lc, "finish");
    }
}
// line-edit hint infomation
std::map<std::string, char *> hint = {
        {"b", "reak"},
        {"s", "tep"},
        {"i", "nfo"},
        {"p", "rint"},
        {"f", "inish"},
        {"ba", "cktrace"},
        {"c", "continue"},
        {"step", "  单步运行，每次执行一行代码"},
        {"backtrace", "  栈回朔，用于观察当前函数调用关系"},
        {"finish", "  单步运行，执行完当前函数剩余的所有代码"},
        {"print", "{variable}  打印变量"},
        {"info registers", " [name] 查看寄存器中的值"},
        {"info breakpoints", "  查看断点信息"},
        {"continue", " 继续执行被调试程序"}

};
char *hints(const char *buf, int *color, int *bold) {
    *color = 35;
    *bold = 0;
    return hint[std::string(buf)];
}