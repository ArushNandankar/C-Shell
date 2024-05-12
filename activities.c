#include "headers.h"

void execute_activities()
{
    for (int i = normalstartidx; i < normalendidx; i++)
    {
        // Print whether the process is running or stopped
        char procpath[1000];
        sprintf(procpath, "/proc/%d/status", normalprocs[i].pid);
        int fd = open(procpath, O_RDONLY);

        if (fd == -1)
        {
            // perror("Failed to open proc status file");
            continue; // Move to the next process if opening failed
        }

        char procstatus[1000];
        ssize_t bytesRead = read(fd, procstatus, sizeof(procstatus) - 1);
        close(fd);

        if (bytesRead == -1)
        {
            perror("Failed to read proc status");
            continue; // Move to the next process if reading failed
        }

        // Null-terminate the string to make it a valid C string
        procstatus[bytesRead] = '\0';

        // Check if the process is running or stopped based on its status
        if (strstr(procstatus, "State:\tR") != NULL || strstr(procstatus, "State:\tS") != NULL)
        {
            printf("%d : %s - running\n", normalprocs[i].pid, normalprocs[i].name);
        }
        else
        {
            printf("%d : %s - stopped\n", normalprocs[i].pid, normalprocs[i].name);
        }
    }
}
