//
// Created by kang on 19-4-27.
//

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

}

char *hints(const char *buf, int *color, int *bold) {
    if (!strcasecmp(buf,"b")) {
        *color = 35;
        *bold = 0;
        return "reak";
    }
    if (!strcasecmp(buf,"i")) {
        *color = 35;
        *bold = 0;
        return "nfo";
    }
    if (!strcasecmp(buf,"p")) {
        *color = 35;
        *bold = 0;
        return "rint";
    }
    if (!strcasecmp(buf,"s")) {
        *color = 35;
        *bold = 0;
        return "tep";
    }
    if (!strcasecmp(buf,"f")) {
        *color = 35;
        *bold = 0;
        return "inish";
    }
    if (!strcasecmp(buf, "ba")) {
        *color = 35;
        *bold = 0;
        return "cktrace";
    }
    if (!strcasecmp(buf, "backtrace") || !strcasecmp(buf, "bt")) {
        *color = 35;
        *bold = 0;
        return " 栈回朔，用于观察当前函数调用关系";
    }
    if (!strcasecmp(buf, "step")) {
        *color = 35;
        *bold = 0;
        return " 单步运行，每次执行一行代码";
    }
    if (!strcasecmp(buf, "step")) {
        *color = 35;
        *bold = 0;
        return " 单步运行，每次执行一行代码";
    }
    if (!strcasecmp(buf, "finish")) {
        *color = 35;
        *bold = 0;
        return " 单步运行，执行完当前函数剩余的所有代码";
    }
    return NULL;
}