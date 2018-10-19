#include <stdio.h>
#include <sys/ptrace.h>
#include <sys/types.h>
#include <unistd.h>
#include <asm/ptrace-abi.h>
#include <sys/reg.h>
#include <sys/wait.h>
int main(int argc, char const *argv[])
{
	pid_t pid;
	pid = fork();
	if (pid == 0) {
		ptrace(PTRACE_TRACEME, 0, NULL, NULL);
		execl("/bin/ls", "ls", NULL);

	} else {
		wait(NULL);
		long ori_eax;
		ori_eax = ptrace(PTRACE_PEEKUSER, pid, 4 * ORIG_RAX, NULL);
		printf("the child process using system call NO.%d", ori_eax);
		ptrace(PTRACE_CONT, pid, NULL, NULL);
	}
	return 0;
}