#include "headers.h"

void execute_command_hasio(char *command, int background, char *home_directory, char *current_directory, char *last_directory, char *last_directory_temp, char *previous_command, char *pepath, char *input_copy, int *time) {
    // case 1 : a > b
    // case 2 : a >> b
    // case 3 : a < b
    // case 31 : a < b > c
    // case 32 : a < b >> c

    command = strtok(command, "\n");

    int count1 = 0; // >
    int count2 = 0; // >>
    int count3 = 0; // <

    char flag[256] = "\0"; // 1 for > // 2 for >> // 3 for <

    int idx = 0;
    while (command[idx] != '\0') {
        if (command[idx] == '>') {
            if (command[idx + 1] == '>') {
                count2++;
                strcat(flag, "2");
                idx++;
            } else {
                count1++;
                strcat(flag, "1");
            }
        } else if (command[idx] == '<') {
            count3++;
            strcat(flag, "3");
        }
        idx++;
    }

    // printf("%s\n", flag);

    if(strcmp(flag, "\0") == 0) {
        // No IO redirection
        execute_command(command, background, home_directory, current_directory, last_directory, last_directory_temp, previous_command, pepath, input_copy, time);
    } else if (strcmp(flag, "1") == 0) {
        // Case 1: a > b
        char *token = strtok(command, ">");
        char *executable = token;
        token = strtok(NULL, ">");
        char *output_file = token;

        // executable = strtok(executable, " \t\n");
        output_file = strtok(output_file, " \t\n");

        // printf("Executing: %s\n", executable);
        // printf("Redirecting output to: %s\n", output_file);

        pid_t pid = fork();

        if (pid < 0) {
            perror("Forking error");
            exit(1);
        } else if (pid == 0) {
            // Child process
            int original_stdout = dup(STDOUT_FILENO); // Save the original stdout

            int fd = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (fd < 0) {
                perror("Error opening file");
                return;
            }

            int fd2 = dup2(fd, STDOUT_FILENO); // Redirect stdout to the file

            close(fd); // Close the file descriptor, as it's no longer needed

            execute_command(executable, background, home_directory, current_directory, last_directory, last_directory_temp, previous_command, pepath, input_copy, time);

            // Restore the original stdout
            dup2(original_stdout, STDOUT_FILENO);
            // Close the duplicated file descriptor (if necessary)
            close(fd2);

            exit(0);
        } else {
            // Parent process
            int status;
            waitpid(pid, &status, 0);
        }
    } else if (strcmp(flag, "2") == 0) {
        // Case 2: a >> b
        char *token = strtok(command, ">>");
        char *executable = token;
        token = strtok(NULL, ">>");
        char *output_file = token;

        // executable = strtok(executable, " \t\n");
        output_file = strtok(output_file, " \t\n");

        // printf("Executing: %s\n", executable);
        // printf("Redirecting output (appending) to: %s\n", output_file);

        pid_t pid = fork();

        if (pid < 0) {
            perror("Forking error");
            exit(1);
        } else if (pid == 0) {
            // Child process
            int original_stdout = dup(STDOUT_FILENO); // Save the original stdout

            int fd = open(output_file, O_WRONLY | O_CREAT | O_APPEND, 0644);
            if (fd < 0) {
                perror("Error opening file");
                return;
            }

            int fd2 = dup2(fd, STDOUT_FILENO); // Redirect stdout to the file

            close(fd); // Close the file descriptor, as it's no longer needed

            execute_command(executable, background, home_directory, current_directory, last_directory, last_directory_temp, previous_command, pepath, input_copy, time);

            // Restore the original stdout
            dup2(original_stdout, STDOUT_FILENO);
            // Close the duplicated file descriptor (if necessary)
            close(fd2);

            exit(0);
        } else {
            // Parent process
            int status;
            waitpid(pid, &status, 0);
        }
    } else if (strcmp(flag, "3") == 0) {
        // Case 3: a < b
        char *token = strtok(command, "<");
        char *executable = token;
        token = strtok(NULL, "<");
        char *input_file = token;

        // executable = strtok(executable, " \t\n");
        input_file = strtok(input_file, " \t\n");

        // printf("Executing: %s\n", executable);
        // printf("Redirecting input from: %s\n", input_file);

        pid_t pid = fork();

        if (pid < 0) {
            perror("Forking error");
            exit(1);
        } else if (pid == 0) {
            // Child process
            int original_stdin = dup(STDIN_FILENO); // Save the original stdout

            int fd = open(input_file, O_RDONLY);
            if (fd < 0) {
                perror("Error opening file");
                return;
            }

            int fd2 = dup2(fd, STDIN_FILENO); // Redirect stdout to the file

            close(fd); // Close the file descriptor, as it's no longer needed

            execute_command(executable, background, home_directory, current_directory, last_directory, last_directory_temp, previous_command, pepath, input_copy, time);

            // Restore the original stdin
            dup2(original_stdin, STDIN_FILENO);
            // Close the duplicated file descriptor (if necessary)
            close(fd2);

            exit(0);
        } else {
            // Parent process
            int status;
            waitpid(pid, &status, 0);
        }
    } else if (strcmp(flag, "31") == 0) {
        // Case 31: a < b > c
        char *token = strtok(command, "<");
        char *executable = token;
        token = strtok(NULL, "<");
        char *input_file = token;
        token = strtok(token, ">");
        token = strtok(NULL, ">");
        char *output_file = token;

        // executable = strtok(executable, " \t\n");
        input_file = strtok(input_file, " \t\n");
        output_file = strtok(output_file, " \t\n");

        // printf("Executing: %s\n", executable);
        // printf("Redirecting input from: %s\n", input_file);
        // printf("Redirecting output to: %s\n", output_file);

        pid_t pid = fork();

        if (pid < 0) {
            perror("Forking error");
            exit(1);
        } else if (pid == 0) {
            // Child process
            int original_stdin = dup(STDIN_FILENO); // Save the original stdout
            int original_stdout = dup(STDOUT_FILENO); // Save the original stdout

            int fd = open(input_file, O_RDONLY);
            if (fd < 0) {
                perror("Error input opening file");
                return;
            }
            int fd2 = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (fd2 < 0) {
                perror("Error opening output file");
                return;
            }

            int fd3 = dup2(fd, STDIN_FILENO); // Redirect stdout to the file
            int fd4 = dup2(fd2, STDOUT_FILENO); // Redirect stdout to the file

            close(fd); // Close the file descriptor, as it's no longer needed
            close(fd2);

            execute_command(executable, background, home_directory, current_directory, last_directory, last_directory_temp, previous_command, pepath, input_copy, time);

            // Restore the original stdin
            dup2(original_stdin, STDIN_FILENO);
            // Restore the original stdout
            dup2(original_stdout, STDOUT_FILENO);
            // Close the duplicated file descriptor (if necessary)
            close(fd3);
            close(fd4);
            exit(0);
        }
    } else if (strcmp(flag, "32") == 0) {
        // Case 32: a < b >> c
        char *token = strtok(command, "<");
        char *executable = token;
        token = strtok(NULL, "<");
        char *input_file = token;
        token = strtok(token, ">>");
        token = strtok(NULL, ">>");
        char *output_file = token;

        // executable = strtok(executable, " \t\n");
        input_file = strtok(input_file, " \t\n");
        output_file = strtok(output_file, " \t\n");

        // printf("Executing: %s\n", executable);
        // printf("Redirecting input from: %s\n", input_file);
        // printf("Redirecting output (appending) to: %s\n", output_file);

        pid_t pid = fork();

        if (pid < 0) {
            perror("Forking error");
            exit(1);
        } else if (pid == 0) {
            // Child process
            int original_stdin = dup(STDIN_FILENO); // Save the original stdout
            int original_stdout = dup(STDOUT_FILENO); // Save the original stdout

            int fd = open(input_file, O_RDONLY);
            if (fd < 0) {
                perror("Error input opening file");
                return;
            }
            int fd2 = open(output_file, O_WRONLY | O_CREAT | O_APPEND, 0644);
            if (fd2 < 0) {
                perror("Error opening output file");
                return;
            }

            int fd3 = dup2(fd, STDIN_FILENO); // Redirect stdout to the file
            int fd4 = dup2(fd2, STDOUT_FILENO); // Redirect stdout to the file

            close(fd); // Close the file descriptor, as it's no longer needed
            close(fd2);

            execute_command(executable, background, home_directory, current_directory, last_directory, last_directory_temp, previous_command, pepath, input_copy, time);

            // Restore the original stdin
            dup2(original_stdin, STDIN_FILENO);
            // Restore the original stdout
            dup2(original_stdout, STDOUT_FILENO);
            // Close the duplicated file descriptor (if necessary)
            close(fd3);
            close(fd4);
            exit(0);
        }
    } else {
        fprintf(stderr, "Invalid use of IO Redirection\n");
        return;
    }
}
