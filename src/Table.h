//
// Created by kang on 19-5-13.
//

#ifndef SRC_TABLE_H
#define SRC_TABLE_H
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <string.h>
#ifdef __cplusplus
extern "C" {
#endif
struct table {
    unsigned int row_num;
    unsigned int col_num;
    unsigned int *col_max_width;
    char ***content;
};

extern void format_table(table *t, char *formated_out);
#define CORNER_FLAG "+"
#define ROW_LINE "-"
#define COL_PADDING " "
#define COL_LINE "|"
static int str_len(unsigned char *str);
#ifdef __cplusplus
}
#endif
#endif //SRC_TABLE_H
