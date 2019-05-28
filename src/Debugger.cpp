//
// Created by kang on 19-4-15.
//

#include "common.h"
#include "Debugger.h"
static std::string toDecString(const std::string &str)
{
    unsigned long value = 0;
    unsigned long power = 1;
    for (int i = str.size() -1; i >= 2; i--) {
        if (str[i] >= '0' && str[i] <= '9')
            value += power * (str[i] - '0');
        else {
            value += power * (str[i] - 'a' + 10);
        }
        power *= 16;
    }
    return getString(value);

}
static std::string toUpper(const std::string &str)
{
    std::string res;
    for (auto i : str) {
        res.push_back(toupper(i));
    }
    return res;
}
static std::string toHexString(const std::string &str)
{
    std::string result;
    return result;

}
static std::string toHexString(long value)
{
    std::stringstream result;
    result << "0x" << std::hex << value;
    return result.str();
}


long Debugger::examRegister(const std::string &reg) {
    std::string reg_name = toUpper(reg);
    long reg_value = ptrace(PTRACE_PEEKUSER, pid, 8 * x64_regs_num[reg_name], NULL);
    if (reg_value == -1)
        perror("examRegister");
    return reg_value;
}
long Debugger::modifyRegister(std::string reg, long value) {
    std::string reg_name = toUpper(reg);
    long ret = ptrace(PTRACE_POKEUSER, pid, 8 * x64_regs_num[reg_name], value);
    if (ret == -1) {
        perror("modifyRegister");
        return C_ERR;
    }
    else
        return C_OK;
}
long Debugger::modifyMemory(const std::string &address, const std::string &value) {
    long addr = 0;
    long v = 0;
    if (address[0] != '0')
        addr = atol(address.c_str());
    else
        addr = atol(toDecString(address).c_str());
    if (value[0] != '0')
        v = atoi(value.c_str());
    else
        v = atol(toDecString(value).c_str());
    modifyMemory(addr, v);
}
long Debugger::modifyMemory(long address, long value) {
    long ret = ptrace(PTRACE_POKETEXT, pid, address, value);
    if (ret == -1){
        perror("modify Memory");
        return C_ERR;
    }
    else
        return C_OK;

}
std::vector<long> Debugger::examMemory(long address, long size) {
    std::vector<long> mem;
    long value;
    for (int i = 0; i < size; i++) {
        value = ptrace(PTRACE_PEEKTEXT, pid, address + i, NULL);
        if (value == -1) {
            perror("exam Memory");
            return mem;
        } else
            mem.push_back(value);
    }
    return mem;

}
long Debugger::examMemory(const std::string &address) {
    long value;
    if (address[0] != '0')
        value = examMemory(atol(address.c_str()));
    else
        value = examMemory(atol(toDecString(address).c_str()));
    printf("address : %s value : %s", address.c_str(), toHexString(value).c_str());
    return value;
}
long Debugger::examMemory(long address) {
    long value = ptrace(PTRACE_PEEKTEXT, pid, address, NULL);
    if (value == -1) {
        perror("exam Memory");
        return C_ERR;
    } else
        return value;
}

Debugger::Debugger(const struct Config &c):pid(NOT_ATTACH) {
    if (c.start_flag == START_PID) {
        this->pid = c.pid;
        attach(pid);
    } else if (c.start_flag == START_FILENAME) {
        this->file_name = std::string(c.file_name);

        this->fd = open(file_name.c_str(), O_RDONLY);
        if (fd == -1)
            perror("Open");
        else {
            if (C_ERR == init()) {
                close(fd);
                this->file_name.clear();
                pid = NOT_ATTACH;
            }
        }
    }
}
Debugger::Debugger(int pid):pid(NOT_ATTACH) {
    attach(pid);
}
Debugger::Debugger(const std::string &file_name) {
    pid = NOT_ATTACH;
    this->file_name = file_name;
    this->fd = open(file_name.c_str(), O_RDONLY);
    if (fd == -1)
        perror("Open");
    else {
        if (C_ERR == init()) {
            close(fd);
            this->file_name.clear();
            pid = NOT_ATTACH;
        }

        //test();
    }

}
long Debugger::init() {
//    std::cout << "init" << fd << std::endl;
    try {
        elf_file = elf::elf{elf::create_mmap_loader(fd)};
        std::shared_ptr<dwarf::loader> p = dwarf::elf::create_loader(elf_file);
        size_t size;
        if (p->load(dwarf::section_type::info, &size) == nullptr) {
            printf("no .debug_info. please add *-g* flag in compile\n");
            return C_ERR;
        } else
            dwarf_file = dwarf::dwarf(p);
    } catch (std::exception &e) {
        std::cerr << e.what() ;
    }


    //千万要用引用
    for (auto &cu : dwarf_file.compilation_units()) {
        for (auto &line : cu.get_line_table()) {
            line_address.insert(std::make_pair(line.line, line.address));
            address_line.insert(std::make_pair(line.address, line.line));
            line_address_set.insert(line.address);
        }
    }

    //todo:被调试的程序名需要与源代码相同
    std::fstream fs;
    fs.open(file_name + ".c", fs.in);
    code_line.push_back(file_name);
    while (!fs.eof()) {
        std::string temp;
        std::getline(fs, temp);
        code_line.push_back(temp);
    }

    return C_OK;

}


int Debugger::trace() {
    if (pid == NOT_ATTACH) {
        pid = fork();
        if (pid == 0) {
            if (-1 == ptrace(PTRACE_TRACEME, 0, NULL, NULL))
                perror("trace me");
            execl(file_name.c_str(), file_name.c_str(), NULL);
        } else {
//            std::cout << "tracee pid " << pid << "filename " << file_name.c_str() << std::endl;
            int status;
            waitpid(pid, &status, 0);
            return status;
        }
    } else {
        ptrace(PTRACE_ATTACH, pid, NULL, NULL);
    }
    return C_OK;

}
int Debugger::stepOver() {
    return C_OK;
}
void Debugger::checkBreakPoint() {
    long rip = examRegister("RIP");
    for (auto i : breakpoint_list) {
        if (line_address[i.first] + 1 == rip) {
            rip--;
            modifyRegister("RIP", rip);
//            printf("now run from breakpoint(rip--)\n");
            // todo:这里把指令0xCC拿走，但这样取消就相当于取消断点
            long now_ins = examMemory(rip);
            *(char *)&now_ins = i.second;
            modifyMemory(rip, now_ins);
            break;
        }

    }
}
int Debugger::stepInto() {
    int status;
    long rip;
    do {
        checkBreakPoint();// todo:设置disable断点
        if (-1 == ptrace(PTRACE_SINGLESTEP, pid , NULL, NULL)) {
            perror("SINGLE step");
            return C_ERR;

        }
        status = waitTracee();
        rip = examRegister("RIP");

//        printSourceLine();

    } while (line_address_set.find(rip) == line_address_set.end());
    return status;

}

int Debugger::stepOut() {
    long rbp = examRegister("RBP");
    long return_address = examMemory(rbp + 8);
    long return_ins = examMemory(return_address);
    long old_ins = return_ins;
    *(char *)&return_ins = INT_ins;
    modifyMemory(return_address, return_ins);
    run();
    int status = waitTracee();
    modifyMemory(return_address, old_ins);
    long rip = examRegister("RIP");
    modifyRegister("RIP", rip - 1);
    return status;
}
long Debugger::setBreakPointInLine(int line_num) {
    long address = line_address[line_num];
    long buf = examMemory(address);
    if (buf == C_ERR) {
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
    if (C_ERR == modifyMemory(address, buf)) {
        perror("set breakpoint");
        return C_ERR;
    }else
        return C_OK;

}
long Debugger::cancelBreakPointInLine(int line_num) {
    for (auto i : breakpoint_list) {
        if (i.first == line_num) {
            i.first = -1;
            long buf = examMemory(line_address[line_num]);
            char *p = (char *)&buf;
            *p = i.second;
            modifyMemory(line_address[line_num], buf);
            return C_OK;
        }
    }
    printf("cancel breakpoint in NO.%d Line\n", line_num);
    return C_ERR;
}

long Debugger::getFuncAddress(std::string func_name) {
    long address;
    for (auto &sec : elf_file.sections()) {
        if (sec.get_hdr().type != elf::sht::symtab && sec.get_hdr().type != elf::sht::dynsym)
            continue;
        for (auto sym : sec.as_symtab()) {
            auto &d = sym.get_data();
            if (d.type() == elf::stt::func && sym.get_name() == func_name) {
                address = d.value;
                return address;
            }
        }
    }
    return C_ERR;
}
long Debugger::setBreakPointInFunc(std::string func_name) {
    long address = getFuncAddress(func_name);
    if (address == C_ERR){
        printf("can't get function address by %s\n", func_name.c_str());
        return C_ERR;
    }
    return setBreakPointInLine(address_line[address]);

}
long Debugger::cancelBreakPointInFunc(std::string func_name) {
    long address = getFuncAddress(func_name);
    if (address == C_ERR){
        printf("can't get function address by %s\n", func_name.c_str());
        return C_ERR;
    }
    return cancelBreakPointInLine(address_line[address]);
}
long Debugger::setBreakPointInStart() {
    int status;
    setBreakPointInFunc("main");
    run();
    status = waitTracee();
    cancelAllBreakPoint();
    //long rip = examRegister("RIP");
    //modifyRegister("RIP", rip - 1);
    if(WIFSTOPPED(status))
        return C_OK;
    else
        return C_ERR;
}
void Debugger::run() {
    checkBreakPoint();
    ptrace(PTRACE_CONT, pid, NULL, NULL);
}
int Debugger::waitTracee() {
    int status;
    waitpid(pid, &status, 0);
    return status;
}

extern void showCallChain(const std::vector<Frame> &frames);
void Debugger::backtrace() {
    //todo:目前没有验证没有-g时能否正常打印
    struct UPT_info *ui;
    unw_addr_space_t as;
    as = unw_create_addr_space(&_UPT_accessors, 0);
    if (!as) {
        printf("unw_create_addr_space failed");
    }
    ui = (struct UPT_info *)_UPT_create(pid);
    if (!ui) {
        printf("_UPT_create failed");
    }



    unw_cursor_t c;
    int rc = unw_init_remote(&c, as, ui);
    if (rc != 0) {
        if (rc == UNW_EINVAL) {
            printf("unw_init_remote: UNW_EINVAL");
        } else if (rc == UNW_EUNSPEC) {
            printf("unw_init_remote: UNW_EUNSPEC");
        } else if (rc == UNW_EBADREG) {
            printf("unw_init_remote: UNW_EBADREG");
        } else {
            printf("unw_init_remote: UNKNOWN");
        }
    }
    std::vector<Frame> frames;
    do {
        Frame frame;
        unw_word_t pc;
        unw_get_reg(&c, UNW_REG_IP, &pc);
        char buf[64]; //Stupid ! Because I dont know how to convert *frame.name*
        (void) unw_get_proc_name(&c, buf, sizeof(buf), &frame.offset);

        strcpy(frame.name, buf);
        strcpy(frame.pc, toHexString(pc).c_str());
        frames.push_back(frame);
        if(strcmp("main", buf) == 0)
            break;
    } while (unw_step(&c) > 0);

    long rbp = examRegister("RBP");
    long rsp = examRegister("RSP");
    for (int i = 0;  i < frames.size(); i++) {
        strcpy(frames[i].rbp, toHexString(rbp).c_str());
        strcpy(frames[i].rsp, toHexString(rsp).c_str());
        rsp = rbp + 8;
        rbp = examMemory(rbp);
        printf("%s[%s + 0x%lx] <- ", frames[i].name, frames[i].name, frames[i].offset);
    }
    printf("Process start\n");
    std::reverse(frames.begin(), frames.end());

    showCallChain(frames);

    _UPT_destroy(ui);
}

//查看修改变量相关代码参考minidbg
uint64_t get_register_value(pid_t pid, reg r) {
    user_regs_struct regs;
    ptrace(PTRACE_GETREGS, pid, nullptr, &regs);
    auto it = std::find_if(begin(g_register_descriptors), end(g_register_descriptors),
                           [r](auto&& rd) { return rd.r == r; });

    return *(reinterpret_cast<uint64_t*>(&regs) + (it - begin(g_register_descriptors)));
}
uint64_t get_register_value_from_dwarf_register (pid_t pid, unsigned regnum) {
    auto it = std::find_if(begin(g_register_descriptors), end(g_register_descriptors),
                           [regnum](auto&& rd) { return rd.dwarf_r == regnum; });
    if (it == end(g_register_descriptors)) {
        throw std::out_of_range{"Unknown dwarf register"};
    }

    return get_register_value(pid, it->r);
}
class ptrace_expr_context : public dwarf::expr_context {
public:
    ptrace_expr_context (pid_t pid) : m_pid{pid} {}

    dwarf::taddr reg (unsigned regnum) override {
        return get_register_value_from_dwarf_register(m_pid, regnum);
    }

    dwarf::taddr pc() override {
        struct user_regs_struct regs;
        ptrace(PTRACE_GETREGS, m_pid, nullptr, &regs);
        return regs.rip;
    }

    dwarf::taddr deref_size (dwarf::taddr address, unsigned size) override {
        //TODO take into account size
        return ptrace(PTRACE_PEEKDATA, m_pid, address, nullptr);
    }

private:
    pid_t m_pid;
};
dwarf::die Debugger::getFunctionDie() {
    long pc = examRegister("RIP");
//    printf("PC:%lx\n", pc);
//    printf("dwarf_file %ld unit\n", dwarf_file.compilation_units().size());

    for (auto &cu : dwarf_file.compilation_units()) {

        if (die_pc_range(cu.root()).contains(pc)) {
            for (const auto& die : cu.root()) {
                if (die.tag == dwarf::DW_TAG::subprogram) {
                    if (die_pc_range(die).contains(pc)) {
                        return die;
                    }
                }
            }
        }
    }

    throw std::out_of_range{"Cannot find function"};
}


long Debugger::modifyVariable(const std::string &name, long value) {
    return C_OK;
}
long Debugger::examVariable(const std::string &name) {
    using namespace dwarf;

    die func = getFunctionDie();

    for (const auto& die : func) {

        if (die.tag == DW_TAG::variable && at_name(die) == name) {
            auto loc_val = die[DW_AT::location];

            //only supports exprlocs for now
            if (loc_val.get_type() == value::type::exprloc ) {
                ptrace_expr_context context {pid};
                auto result = loc_val.as_exprloc().evaluate(&context);

                switch (result.location_type) {
                    case expr_result::type::address:
                    {
                        auto value = examMemory(result.value);
                        std::cout << at_name(die) << " (0x" << std::hex << result.value << ") = 0x" << std::hex << value << std::endl;
                        break;
                    }

                    case expr_result::type::reg:
                    {
                        auto value = get_register_value_from_dwarf_register(pid, result.value);
                        std::cout << at_name(die) << " (reg " << result.value << ") = " << value << std::endl;
                        break;
                    }

                    default:
                        throw std::runtime_error{"Unhandled variable location"};
                }
            } else {
                throw std::runtime_error{"Unhandled variable location"};
            }
            return C_OK;
        }
    }
    return C_ERR;
}

void Debugger::cancelAllBreakPoint() {
    checkBreakPoint();
    for (auto i : breakpoint_list) {
        long now_ins = ptrace(PTRACE_PEEKTEXT, pid, line_address[i.first], NULL);
        if (*(char *)&now_ins == INT_ins)
            *(char *)&now_ins = i.second;
        else
            continue;
        ptrace(PTRACE_POKETEXT, pid, line_address[i.first], now_ins);
    }
    breakpoint_list.clear();
}
void Debugger::detach() {
    cancelAllBreakPoint();
    ptrace(PTRACE_DETACH, pid, NULL, NULL);
    pid = NOT_ATTACH;
    close(fd);
    //todo:析构elf文件对象和dwarf对象

}
long Debugger::attach(int pid) {
    if (this->pid != NOT_ATTACH) {
        printf("Now is attach process. PID is %d\n", this->pid);
        return C_ERR;
    } else {
        this->pid = pid;
        std::string str = getString(pid);
        file_name = "/proc/" + str + "/exe";
        this->fd = open(file_name.c_str(), O_RDONLY);
        if (fd == -1){
            perror("Open");
            return C_ERR;
        } else {
            init();
        }
        return C_OK;
    }

}

void Debugger::printLineAddress() {
    printf("****printLineAddress****\n");
    for (auto &i : line_address) {
        printf("line %d : address %lx\n", i.first, i.second);
    }
}
void Debugger::printSourceLine() {
    long rip = examRegister("RIP");
    if (line_address_set.find(rip) != line_address_set.end())
        printf("<%d>  %s\n", address_line[rip], code_line[address_line[rip]].c_str());
    else
        ;
        //printf("RIP:%lx\n", rip);
}
void Debugger::printBreakPointList() {
    for(auto &i : breakpoint_list) {
        printf("breakpoint in line:%d\n", i.first);
    }
    //    for (auto &i : line_address_set)
    //        printf("line_address set : %lx\n", i);

}
pid_t Debugger::getTracePid() {
    return pid;
}
bool Debugger::isDwarfFile() {
    elf::elf elf_file = elf::elf(elf::create_mmap_loader(fd));
    std::shared_ptr<dwarf::loader> p = dwarf::elf::create_loader(elf_file);
    size_t size;
    if (!p->load(dwarf::section_type::info, &size))
        return false;
    else
        return true;
}

