//
// Created by kang on 19-4-27.
//

#ifndef SRC_DISPLAYER_H
#define SRC_DISPLAYER_H

#include <cstddef>
#include <string.h>
#include <linenoise.h>

void completion(const char *buf, linenoiseCompletions *lc) ;
char *hints(const char *buf, int *color, int *bold) ;


#endif //SRC_DISPLAYER_H
