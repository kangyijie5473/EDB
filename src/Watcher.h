//
// Created by kang on 19-4-27.
//

#ifndef SRC_WATCHER_H
#define SRC_WATCHER_H

#include <unistd.h>
#include <sys/user.h>
#include <fcntl.h>
#include <dirent.h>
#include <string>
#include <cstring>
#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include "common.h"



class Watcher {
public:
    Watcher();
    explicit Watcher(pid_t pid);
    void getOpenFd();
    int attach(pid_t pid);
    void detach();
    void getStackHeapAddress();
    void getCwd();
    void getArgs();
    void getUsedMem();
    void getIoInfo();
    void getThreadsID();
private:
    pid_t pid;
    std::string file_path;
};

extern void split(std::string& s, std::string& c, std::vector< std::string >& v);

static void trimUnusedSpace(std::string &str)
{
    for (auto i = str.begin(); i != str.end(); ) {
        auto j = i + 1;
        if (*i == ' ' && *j == ' ')
            str.erase(j);
        else
            i++;
    }
}

#endif //SRC_WATCHER_H
