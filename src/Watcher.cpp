//
// Created by kang on 19-4-27.
//

#include "common.h"
#include "Watcher.h"

Watcher::Watcher():pid(NOT_ATTACH) {

}
Watcher::Watcher(int pid) :pid(NOT_ATTACH){
    if (C_ERR == attach(pid))
        this->pid = NOT_ATTACH;
}
int Watcher::attach(int pid) {
    if (this->pid != NOT_ATTACH) {
        return C_ERR;
    }
    file_path = "/proc/" + getString(pid);
    this->pid = pid;
    if (-1 == access(file_path.c_str(), X_OK)) {
        perror("access");
        file_path.clear();
        this->pid = NOT_ATTACH;
        return C_ERR;
    }
    return C_OK;
}
void Watcher::detach() {
    pid = NOT_ATTACH;
    file_path.clear();
}
void Watcher::getOpenFd() {
    std::string openfd_dir = file_path + "/fd";
    DIR *dir = opendir(openfd_dir.c_str());
    if (dir == NULL) {
        perror("opendir: /fd");
        return;
    }
    struct dirent *dp;
    char buffer[100]= {0};
    while (true) {
        dp = readdir(dir);
        if (dp == NULL)
            break ;
        if (strcmp(dp->d_name, ".") == 0 || strcmp(dp->d_name, "..") == 0)
            continue;
        printf("%s -> ", dp->d_name);
        std::string fd_file = openfd_dir + "/" + dp->d_name;
        if (-1 == readlink(fd_file.c_str(), buffer, 100)) {
            perror("readlink:");
            return;
        }
        printf("%s\n", buffer);
        bzero(buffer, 100);
    }
    closedir(dir);
}
void Watcher::getArgs() {
    std::string cmdline = file_path + "/cmdline";
    int fd = open(cmdline.c_str(), O_RDONLY);
    char buffer[100] = {0};
    int n = read(fd, buffer, 100);
    if (n == -1) {
        perror("read ");
        close(fd);
        return;
    }
    printf("args is %s\n", buffer);
    close(fd);
}
void Watcher::getCwd() {
    std::string cwd = file_path + "/cwd";
    char buffer[100] = {0};
    if (-1 == readlink(cwd.c_str(), buffer, 100)) {
        perror("readlink cwd:");
        return;
    }
    printf("now process in %s dir\n", buffer);
}
void Watcher::getUsedMem() {
    std::string statm_file = file_path + "/statm";
    long size, resident, share, text, lib, data, dirtypage;
    std::fstream fs;
    fs.open(statm_file, fs.in);
    fs >> size >> resident >> share >> text >> lib >> data >> dirtypage;
    printf("now process is using %ld kbytes memory，page size %ld kbytes\n", resident * PAGE_SIZE / 1024, PAGE_SIZE/ 1024) ;

}
void Watcher::getIoInfo() {
    long read_char, write_char, syscall_read_char, syscall_write_char, real_read, real_write, cancelled_write;
    std::string buffer;
    std::fstream fs;
    std::string io_file = file_path + "/io";
    fs.open(io_file, fs.in);
    fs >> buffer >> read_char >> buffer >> write_char >> buffer
       >> syscall_read_char >> buffer >> syscall_write_char >> buffer >> real_read >> buffer >> real_write >> buffer >> cancelled_write;
    printf("read %ld bytes from storage, write %ld bytes from storage", real_read, real_write);
    fs.close();
}

void Watcher::getThreadsID() {
    std::string threads_dir = file_path + "/task";
    DIR *dir = opendir(threads_dir.c_str());
    if (dir == NULL) {
        perror("opendir: /task");
        return;
    }
    struct dirent *dp;
    while (true) {
        dp = readdir(dir);
        if (dp == NULL)
            break ;
        if (strcmp(dp->d_name, ".") == 0 || strcmp(dp->d_name, "..") == 0)
            continue;
        printf("%s", dp->d_name);
    }
    closedir(dir);
}
void Watcher::getStackHeapAddress() {
    std::string maps_file = file_path + "/maps";
    std::fstream fs;
    fs.open(maps_file, fs.in);
    if (!fs.is_open()) {
        std::cout << "error open" << std::endl;
    }
    std::string content;
    int flag = 0;
    std::vector<std::string> v;
    std::string space = " ";
    while (!fs.eof() && flag != 2) {
        std::getline(fs, content);
        trimUnusedSpace(content);
        split(content, space, v);
//        std::cout << content << "size : "<< v.size() <<std::endl;
//        std::cout << v.size() << std::endl;
        if (v[5] == "[heap]" || v[5] == "[stack]") {
            flag++;
            std::cout << v[5] << "  address is 0x" << v[0] << std::endl;
        }
        v.clear();
    }
    fs.close();

}