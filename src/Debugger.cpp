//
// Created by kang on 19-4-15.
//

#include "Debugger.h"
static std::string getSring(int n)
{
    std::stringstream newstr;
    newstr << n;
    return newstr.str();
}

long Debugger::examRegister(const std::string &reg) {
    std::cout << "exam Register " << pid << std::endl;
    sleep(10);
    long reg_value = ptrace(PTRACE_PEEKUSER, pid, 8 * x64_regs_num[reg], NULL);
    if (reg_value == -1)
        perror("examRegister");
    return reg_value;
}
long Debugger::modifyRegister(std::string reg, long value) {
    long ret = ptrace(PTRACE_POKEUSER, pid, 8 * x64_regs_num[reg], value);
    if (ret == -1)
        perror("modifyRegister");
    else
        return C_OK;
}
long Debugger::modifyMemory(long address, std::string value) {
    long v = atol(value.c_str());
    long ret = ptrace(PTRACE_POKETEXT, pid, address, v);
    if (ret == -1)
        perror("modify Memory");
    else
        return C_OK;

}
std::vector<long> Debugger::examMemory(long address, long size) {
    std::vector<long> mem;
    long value;
    for (int i = 0; i < size; i++) {
        value = ptrace(PTRACE_PEEKTEXT, pid, address + i, NULL);
        if (value == -1) {
            printf("exam PID %d\n", pid);
            perror("exam Memory");
            return mem;
        } else
            mem.push_back(value);
    }
    return mem;

}

Debugger::Debugger(int pid) {
    this->pid = pid;
    std::string str = getSring(pid);
    file_name = "/proc/" + str + "/exe";
    this->fd = open(file_name.c_str(), O_RDONLY);
    if (fd == -1)
        perror("Open");
    else {
        init();
    }
}
Debugger::Debugger(const std::string &file_name) {
    pid = NOT_ATTACH;
    this->file_name = file_name;
    this->fd = open(file_name.c_str(), O_RDONLY);
    if (fd == -1)
        perror("Open");
    else {
        init();
    }

}
long Debugger::init() {
    std::cout << "init" << fd << std::endl;
    try {
        elf_file = elf::elf(elf::create_mmap_loader(fd));
        std::cout << "init end" << std::endl;

        dwarf_file = dwarf::dwarf(dwarf::elf::create_loader(elf_file));
    } catch (std::exception &e) {
        std::cerr << e.what() ;
    }



    for (auto cu : dwarf_file.compilation_units()) {
        for (auto &line : cu.get_line_table()) {
            line_address.insert(std::make_pair(line.line, line.address));
        }
    }
//    std::cout << "list line address" << std::endl;
//    for (auto i : line_address)
//        std::cout << i.first << "  " << i.second << std::endl;
    return C_OK;

}


long Debugger::trace() {
    if (pid == NOT_ATTACH) {
        pid = fork();
        if (pid == 0) {

            if (-1 == ptrace(PTRACE_TRACEME, 0, NULL, NULL))
                perror("trace me");
            execl(file_name.c_str(), file_name.c_str(), NULL);
        } else {
            std::cout << "father pid " << pid << "filename " << file_name.c_str() << std::endl;
            int status;
            waitpid(pid, &status, 0);
            std::cout << "wait status " << status << std::endl;
        }
    } else {
        ptrace(PTRACE_ATTACH, pid, NULL, NULL);
    }
    return C_OK;

}
void Debugger::singleStep() {

}
long Debugger::setBreakPointInLine(long line_num) {
    long address = line_address[line_num];
    long buf = ptrace(PTRACE_PEEKTEXT, pid, address, NULL);
    if (buf == -1) {
        perror("set breakPoint");
        return C_ERR;
    }

    char *p = (char *)&buf;
    if (*p == INT_ins) {
        printf("err:breakpoint is dup in No.%d Line\n", line_num);
        return C_ERR;
    }
    char old_ins = *p;
    *p = INT_ins;

    breakpoint_list.push_back(std::make_pair(line_num, old_ins));
    if (-1 == ptrace(PTRACE_POKETEXT, pid, address, buf)) {
        perror("set breakpoint");
        return C_ERR;
    }else
        return C_OK;

}
long Debugger::cancelBreakPointInLine(long line_num) {
    for (auto i : breakpoint_list) {
        if (i.first == line_num) {
            i.first = -1;
            long buf = ptrace(PTRACE_PEEKTEXT, pid, line_address[line_num], NULL);
            char *p = (char *)&buf;
            *p = i.second;
            ptrace(PTRACE_POKETEXT, pid, line_address[line_num], buf);
            return C_OK;
        }
    }
    printf("cancel breakpoint in NO.%ld Line\n", line_num);
    return C_ERR;
}
void Debugger::run() {
    long rip = ptrace(PTRACE_PEEKUSER, pid, 8 * x64_regs_num["RIP"], NULL);
    for (auto i : breakpoint_list) {
        if (line_address[i.first] + 1 == rip) {
            rip--;
            ptrace(PTRACE_POKEUSER, pid, 8 * x64_regs_num["RIP"], rip);
            printf("now run from breakpoint(rip--)\n");
            // todo:这里应该把指令0xCC拿走，但这样取消就相当于取消断点
            break;
        }

    }
    ptrace(PTRACE_CONT, pid, NULL, NULL);
}
int Debugger::waitTracee() {
    int status;
    waitpid(pid, &status, 0);
    return status;
}
void Debugger::backtrace() {

}
long Debugger::examVariable(std::string name) {

}
long Debugger::modifyVariable(std::string name, long value) {

}
