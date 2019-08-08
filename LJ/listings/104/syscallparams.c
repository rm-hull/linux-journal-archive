#include <sys/ptrace.h> 
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <linux/user.h>     /* For constants ORI_EAX etc */
#include <sys/syscall.h>    /* For SYS_write etc */

int main()
{   pid_t child;
    long orig_eax, eax;
    long params[3];
    int status;
    int insyscall = 0;

    child = fork();
    if(child == 0) {
        ptrace(PTRACE_TRACEME, 0, NULL, NULL);
        execl("/bin/ls", "ls", NULL);
    }
    else {
        while(1) {
            wait(&status);
            if(WIFEXITED(status))
                break;
            orig_eax = ptrace(PTRACE_PEEKUSER, child, 4 * ORIG_EAX, NULL);
            if(orig_eax == SYS_write) {
                if(insyscall == 0) {    /* Syscall entry */
                    insyscall = 1;
                    params[0] = ptrace(PTRACE_PEEKUSER, child, 4 * EBX, NULL);
                    params[1] = ptrace(PTRACE_PEEKUSER, child, 4 * ECX, NULL);
                    params[2] = ptrace(PTRACE_PEEKUSER, child, 4 * EDX, NULL);

                    printf("Write called with %ld, %ld, %ld\n", 
                        params[0], params[1], params[2]);
                }
                else { /* Syscall exit */ 
                    eax = ptrace(PTRACE_PEEKUSER, child, 4 * EAX, NULL);
                    printf("Write returned with %ld\n", eax);
                    insyscall = 0;
                }
            }
            ptrace(PTRACE_SYSCALL, child, NULL, NULL);
        }
    }
    return 0;
}
