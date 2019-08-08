#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <linux/user.h>
#include <stdio.h>

const int long_size = sizeof(long);

void getdata(pid_t child, long addr, char *str, int len)
{   char *laddr;
    int i, j;
    union u {
            long val;
            char chars[long_size];
    }data;
    
    i = 0;
    j = len / long_size;
    laddr = str;
    while(i < j) {
        data.val = ptrace(PTRACE_PEEKDATA, child, addr + i * 4, NULL);
        memcpy(laddr, data.chars, long_size);
        ++i;
        laddr += long_size;
    }
    
    j = len % long_size;
    if(j != 0) {
        data.val = ptrace(PTRACE_PEEKDATA, child, addr + i * 4, NULL);
        memcpy(laddr, data.chars, j);
    }
    str[len] = '\0';       
}

void putdata(pid_t child, long addr, char *str, int len)
{   char *laddr;
    int i, j;
    union u {
            long val;
            char chars[long_size];
    }data;
    
    i = 0;
    j = len / long_size;
    laddr = str;
    while(i < j) {
        memcpy(data.chars, laddr, long_size);
        ptrace(PTRACE_POKEDATA, child, addr + i * 4, data.val);
        ++i;
        laddr += long_size;
    }
    
    j = len % long_size;
    if(j != 0) {
        memcpy(data.chars, laddr, j);
        ptrace(PTRACE_POKEDATA, child, addr + i * 4, data.val);
    }
}

long freespaceaddr(pid_t pid)
{
    FILE *fp;
    char filename[30];
    char line[85];
    long addr;
    char str[20];

    sprintf(filename, "/proc/%d/maps", pid);
    fp = fopen(filename, "r");
    if(fp == NULL)
        exit(1);
    while(fgets(line, 85, fp) != NULL) {
        sscanf(line, "%lx-%*lx %*s %*s %s", &addr, str, str, str, str);
        if(strcmp(str, "00:00") == 0)
            break;
    }
    fclose(fp);
    return addr;
}

int main(int argc, char *argv[])
{   pid_t traced_process;
    struct user_regs_struct oldregs, regs;
    long ins;
    int len = 41;
    char insertcode[] = "\xeb\x15\x5e\xb8\x04\x00\x00\x00\xbb\x02\x00\x00\x00\x89\xf1\xba\x0c\x00\x00\x00\xcd\x80\xcc\xe8\xe6\xff\xff\xff\x48\x65\x6c\x6c\x6f\x20\x57\x6f\x72\x6c\x64\x0a\x00";
    char backup[len];
    long addr;

    if(argc != 2) {
        printf("Usage: %s <pid to be traced>\n", argv[0], argv[1]);
        exit(1);
    }

    traced_process = atoi(argv[1]);

    ptrace(PTRACE_ATTACH, traced_process, NULL, NULL);
    wait(NULL);

    ptrace(PTRACE_GETREGS, traced_process, NULL, &regs);
    addr = freespaceaddr(traced_process);
    getdata(traced_process, addr, backup, len);
    
    putdata(traced_process, addr, insertcode, len);
    memcpy(&oldregs, &regs, sizeof(regs));
    regs.eip = addr;
    ptrace(PTRACE_SETREGS, traced_process, NULL, &regs);
    ptrace(PTRACE_CONT, traced_process, NULL, NULL);
    
    wait(NULL);
    printf("The process stopped, Putting back the original instructions\n");
    putdata(traced_process, addr, backup, len);
    ptrace(PTRACE_SETREGS, traced_process, NULL, &oldregs);
    printf("Letting it continue with original flow\n");
    ptrace(PTRACE_DETACH, traced_process, NULL, NULL);
    
    return 0;
}
