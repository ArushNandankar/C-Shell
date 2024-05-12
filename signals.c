#include "headers.h"

void sigint_handler(int signum)
{
    // printf("Ctrl+C (SIGINT) received.\n");
    // Add your handling code here
    if (fgpid != -1)
    {
        kill(fgpid, SIGINT);
        printf("\n");
    }
}

void eof_handler(int signum)
{
    // printf("Ctrl+D (EOF) received.\n");
    // Add your handling code here
    for (int i = normalstartidx; i < normalendidx; i++)
    {
        kill(normalprocs[i].pid, SIGKILL);
    }
    exit(0);
}

void sigtstp_handler(int signum)
{
    // push any foreground process to background and stop it
    if(fgpid != -1)
    {
        kill(fgpid, SIGTSTP);
        fgpid = -1;
    }
}