//
// Created by kang on 19-4-27.
//

#ifndef SRC_WATCHER_H
#define SRC_WATCHER_H

#include <unistd.h>
#include <string>
#include <fcntl.h>
#include <dirent.h>
#include <cstring>
#include <iostream>
#include <sstream>
#include <vector>

const int NO_ATTACH = -1;
const int C_ERR = -1;
const int C_OK = 0;
static std::string getString(int n)
{
    std::stringstream newstr;
    newstr << n;
    return newstr.str();
}
class Watcher {
public:
    Watcher();
    explicit Watcher(int pid);
    void getOpenFd();
    int attach(int pid);
    void detach();
    void getStackHeapAddress();
    void getCwd();
    void getArgs();
    void getUsedMem();
    void getIoInfo();
    void getThreadsID();
    void getProcessTree();
private:
    pid_t pid;
    std::string file_path;
};
static void split(std::string& s, std::string& c, std::vector< std::string >& v)
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
