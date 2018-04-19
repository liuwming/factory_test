#include "util.h"
#include <sys/types.h>
#include <signal.h>

int rtt_exec(char* cmd, int sync)
{
    pid_t pid;

    if (sync)
        return system(cmd);
    else {
        pid = fork();
        if (pid < 0) return -1;
        else if (pid) {
            waitpid(pid, NULL, 0);
            return 0;
        } else {
            pid = fork();       /* fork twice to avoid zombie */
            if (pid == 0) {
                execl("/bin/bash", "bash", "-c", cmd, (char*)NULL);
            } else 
                exit(0);
        }
    }
    return 0;
}

pid_t rtt_exec2(char* cmd)
{
    pid_t pid;
    int maxfd, i;

    pid = fork();
    /* return if parent or error */
    if (pid < 0 || pid > 0) return pid; 

    /* close all fd and start long run program */
    maxfd=sysconf(_SC_OPEN_MAX);
    for(i = 0; i < maxfd; i++)
        close(i);

    execl("/bin/bash", "bash", "-c", cmd, (char*)NULL);

    return 0;
}

void rtt_wait_pid(pid_t pid)
{
    kill(pid, SIGKILL);
    waitpid(pid, NULL, 0);
}

void* xalloc(size_t size)
{
    void* ptr = calloc(1, size);
    PANIC_IF(!ptr);
    return ptr;
}

