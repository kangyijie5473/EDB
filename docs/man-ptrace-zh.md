### Name
ptrace - process trace
### SYNOPSIS
```c

include <sys/ptrace.h>

long ptrace(enum __ptrace_request request, pid_t pid, void *addr, void *data);
```
### DESCRIPTION
ptrace 系统调用提供了一个方法，让某个进程（称为追踪者，tracer）可以观测和控制另一个进程的执行（被追踪者，tracee），并且可以查看，修改tracee的内存，寄存器中的值。
主要用来实现断点调试和系统调用追踪。

一个tracee首先需要被一个tracer连接（attach）上。随后的指令都是针对线程的：在一个多线程程序中，每个线程都是被单独的tracer进行attach的，如果没被attach也就是没被debug的。
因此，tracee意味着一个线程。ptrace命令使用如下的格式被发送给特定的tracee。
    ptrace(PTRACE_foo, pid, ...)
这里的`pid`就是对应的Linux 线程ID。



