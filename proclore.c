#include "headers.h"

// obtain the information regarding the current shell
void execute_proclore()
{
    char pid[10];
    sprintf(pid, "%d", getpid());
    execute_proclore_pid(pid);
}


void execute_proclore_pid(char *pid)
{
    printf("pid: %s\n", pid);

    char path[1000];
    sprintf(path, "/proc/%s/stat", pid);
    int fd = open(path, O_RDONLY);
    if (fd < 0)
    {
        perror("Error opening file");
        return;
    }
    char buffer[1000];
    read(fd, buffer, sizeof(buffer));

    char *token = strtok(buffer, " ");
    int i = 0;
    int processGroup = -1; // Initialize process group to an invalid value

    char pstat[1000];
    char execPath[1000];

    while (token != NULL)
    {
        if (i == 2)
        {
            // Process Status
            sprintf(pstat, "Process Status: %s", token);
        }
        else if (i == 4)
        {
            // Process Group
            processGroup = atoi(token);
            printf("Process Group: %s\n", token);
        }
        else if (i == 22)
        {
            // Virtual Memory Size
            printf("Virtual Memory Size: %s\n", token);
        }
        else if (i == 1)
        {
            // Extract and print the complete executable path
            char exePathBuffer[1000];
            snprintf(exePathBuffer, sizeof(exePathBuffer), "/proc/%s/exe", pid);
            ssize_t len = readlink(exePathBuffer, execPath, sizeof(execPath) - 1);
            if (len != -1)
            {
                execPath[len] = '\0';
                printf("Executable Path: %s\n", execPath);
            }
        }
        token = strtok(NULL, " ");
        i++;
    }

    // Check if the process is in the foreground
    if (processGroup == tcgetpgrp(STDOUT_FILENO))
    {
        //printf("Foreground Process: Yes\n");
        printf("%s+\n", pstat);
    }
    else
    {
        //printf("Foreground Process: No\n");
        printf("%s\n", pstat);
    }

    close(fd);
}
