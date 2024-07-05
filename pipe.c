#include "headers.h"

void execute_command_haspipes(char *command, int background, char *home_directory, char *current_directory, char *last_directory, char *last_directory_temp, char *previous_command, char *pepath, char *input_copy, int *time) {
    command = strtok(command, "\n");
    // tokenise the command by |
    char *token = strtok(command, "|");
    char *commands[100];
    int i = 0;
    while (token != NULL) {
        commands[i] = token;
        i++;
        token = strtok(NULL, "|");
    }
    commands[i] = NULL;
    
    // printf("%d\n", i);

    int fd[2];
    int fd_in = 0;
    int j = 0;
    int pid;
    while (commands[j] != NULL) {
        pipe(fd);
        pid = fork();
        if (pid == 0) {
            dup2(fd_in, 0);
            if (commands[j + 1] != NULL)
            {
                dup2(fd[1], 1);
            }
            close(fd[0]);
            execute_command_hasio(commands[j], background, home_directory, current_directory, last_directory, last_directory_temp, previous_command, pepath, input_copy, time);
            exit(0);
        } else {
            wait(NULL);
            close(fd[1]);
            fd_in = fd[0];
            j++;
        }
    }
}
