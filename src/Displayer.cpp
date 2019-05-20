//
// Created by kang on 19-4-27.
//

#include <map>
#include "Displayer.h"
#include "Table.h"

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
        {"c", "ontinue"},
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
void showCallChain(const std::vector<Frame> &frames)
{
    struct table frame_table;
    frame_table.row_num = frames.size() * 3 + 1;
    frame_table.col_num = 4;
    frame_table.col_max_width = (unsigned int *)malloc(sizeof(int) * frame_table.col_num);
    frame_table.content = (char ***)malloc(sizeof(char **) * frame_table.row_num);
    for (int i = 0; i < frame_table.row_num; i++) {
        frame_table.content[i] = (char **)malloc(sizeof(char *) * frame_table.col_num);
    }
    //
    frame_table.content[0][0] = "栈帧名";
    frame_table.content[0][1] = "说明";
    frame_table.content[0][2] = "地址";
    frame_table.content[0][3] = "值";
    for (int j = 0; j < frames.size(); j++) {
        int i = j * 3 + 1;
        frame_table.content[i][0] = " ";
        frame_table.content[i][1] = "RBP";
        frame_table.content[i][2] = const_cast<char *>(frames[j].rbp);
        if (j == 0) {
            frame_table.content[i][3] = ".....";
        } else {
            frame_table.content[i][3] = const_cast<char *>(frames[j - 1].rbp);
        }
        i++;
        frame_table.content[i][0] = const_cast<char *>(frames[j].name);
        frame_table.content[i][1] = ".....";
        frame_table.content[i][2] = "...........";
        frame_table.content[i][3] = "............";

        i++;
        if (j == frames.size() - 1) {
            frame_table.content[i][1] = "RSP";
            frame_table.content[i][2] = const_cast<char *>(frames[j].rsp);
            frame_table.content[i][3] = "****";
        } else {
            frame_table.content[i][1] = "RIP";
            frame_table.content[i][2] = const_cast<char *>(frames[j].rsp);
            frame_table.content[i][3] = const_cast<char *>(frames[j].pc);

        }
        frame_table.content[i][0] = " ";
    }
    char formated_out[4096] = {0};
    //
    format_table(&frame_table, formated_out);
    printf("%s", formated_out);

    free(frame_table.col_max_width);
    for (int i = 0; i < frame_table.row_num; i++) {
        free(frame_table.content[i]);
    }
    free(frame_table.content);
}
