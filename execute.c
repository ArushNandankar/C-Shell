#include "headers.h"

void execute_command(char *command, int background, char *home_directory, char *current_directory, char *last_directory, char *last_directory_temp, char *previous_command, char *pepath, char *input_copy, int *time)
{
    command = strtok(command, "\n");

    // printf("Command is %s\n", command);

    char *args[256];
    int arg_count = 0;

    // Tokenize the command
    char *token = strtok(command, " \t\n");
    while (token != NULL)
    {
        args[arg_count++] = token;
        token = strtok(NULL, " \t\n");
    }
    args[arg_count] = NULL;

    if (arg_count == 0)
    {
        return;
    }

    if (strcmp(args[arg_count - 1], "&") == 0)
    {
        background = 1;
        args[--arg_count] = NULL;
    }

    if (arg_count == 0)
    {
        return;
    }

    struct timeval start, end;
    gettimeofday(&start, NULL);

    if (strcmp(args[0], "fg") == 0)
    {
        if (arg_count != 2)
        {
            fprintf(stderr, "ERROR: Usage fg <pid>\n");
            return;
        }

        int fgkpid = atoi(args[1]);

        fgpid = fgkpid;

        execute_fg(fgkpid);

        fgpid = -1;

        // end clock
        gettimeofday(&end, NULL);
        double elapsed_time = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1000000.0;
        // printf("Child process took %.6f seconds.\n", elapsed_time);
        *time = (int)elapsed_time;
        // printf("%d\n", *time);

        return;
        // exit(EXIT_SUCCESS);
    }

    if (strcmp(args[0], "warp") == 0)
    {
        // Execute the 'warp' command
        if (getcwd(last_directory_temp, sizeof(char) * 256) != NULL)
        {
            // printf("Current dir: %s\n", current_directory);
            // printf("Last dir: %s\n", last_directory);
        }
        else
        {
            perror("getcwd() error");
        }
        execute_warp(args[1], home_directory, last_directory);

        strcpy(last_directory, last_directory_temp);
        return;
    }

    pid_t pid = fork();

    if (pid < 0)
    {
        perror("Forking error");
        exit(1);
    }

    if (background == 1)
    {
        fgpid = -1;
    }
    else
    {
        fgpid = pid;
    }

    if (pid == 0)
    {
        // Child process
        if (strcmp(args[0], "peek") == 0)
        {
            // Execute the 'peek' command
            int show_hidden = 0;
            int show_info = 0;
            const char *path = "."; // Default path is current directory

            for (int i = 1; i < arg_count; i++)
            {
                if (strcmp(args[i], "-a") == 0)
                {
                    show_hidden = 1;
                }
                else if (strcmp(args[i], "-l") == 0)
                {
                    show_info = 1;
                }

                else if (strcmp(args[i], "-al") == 0 || strcmp(args[i], "-la") == 0)
                {
                    show_info = 1;
                    show_hidden = 1;
                }

                else
                {
                    path = args[i];
                }
            }
            execute_peek(path, show_hidden, show_info, home_directory, last_directory);
            exit(EXIT_SUCCESS);
        }

        else if (strcmp(args[0], "seek") == 0)
        {
            int fileflag = 0;
            int directoryflag = 0;
            int eflag = 0;
            char search[256] = "\0";
            char dirtosearch[256] = "\0";

            int flagcount = 0;
            int searchflag = 0;
            int dirtosearchflag = 0;

            if (arg_count == 1)
            {
                fprintf(stderr, "ERROR: Usage seek <flags> <search> <target_directory>\n");
                return;
            }

            for (int i = 1; i < arg_count; i++)
            {
                if (strcmp(args[i], "-d") == 0)
                {
                    directoryflag = 1;
                    flagcount++;
                }
                else if (strcmp(args[i], "-f") == 0)
                {
                    fileflag = 1;
                    flagcount++;
                }
                else if (strcmp(args[i], "-e") == 0)
                {
                    eflag = 1;
                    flagcount++;
                }

                else
                {
                    if (searchflag == 0)
                    { // search name is the argument;
                        strcpy(search, args[i]);
                        searchflag = 1;
                    }
                    else
                    {
                        // search directory is the argument
                        strcpy(dirtosearch, args[i]);
                        dirtosearchflag = 1;
                    }
                }

                if (directoryflag == 1 && fileflag == 1)
                {
                    fprintf(stderr, "ERROR: Cannot use both -d and -f flags together\n");
                    return;
                }
            }

            if (searchflag == 0)
            {
                fprintf(stderr, "ERROR: Usage seek <flags> <search> <target_directory>\n");
                return;
            }

            int *count = malloc(sizeof(int));
            *count = 0;

            char *onlyfile = malloc(sizeof(char) * 256);
            execute_seek(search, dirtosearch, fileflag, directoryflag, eflag, home_directory, last_directory, count, onlyfile);

            // printf("count is %d\n", *count);

            if (*count == 1 && eflag == 1)
            {
                // if onlyfile is directory then warp to that directory else if it is file then open it
                struct stat file_stat;
                if (stat(onlyfile, &file_stat) < 0)
                {
                    perror("ERROR: Could not get file stats\n");
                    return;
                }
                if (S_ISDIR(file_stat.st_mode))
                {
                    // // using chdir
                    // if (chdir(onlyfile) != 0)
                    // {
                    //     perror("ERROR: Could not change directory\n");
                    //     return;
                    // }
                    char tcmd[256];
                    sprintf(tcmd, "warp %s", onlyfile);
                    execute_command(tcmd, 0, home_directory, current_directory, last_directory, last_directory_temp, previous_command, pepath, input_copy, time);
                }
                else if (S_ISREG(file_stat.st_mode))
                {
                    // if it a file then print the contents of the file using system calls
                    int fd = open(onlyfile, O_RDONLY);
                    if (fd < 0)
                    {
                        perror("ERROR: Could not open file\n");
                        return;
                    }
                    char *buffer = malloc(sizeof(char) * 256);
                    int bytes_read = read(fd, buffer, 256);
                    if (bytes_read < 0)
                    {
                        perror("ERROR: Could not read file\n");
                        return;
                    }
                    printf("%s\n", buffer);
                    close(fd);
                }
            }
            exit(EXIT_SUCCESS);
        }

        else if (strcmp(args[0], "pastevents") == 0)
        {
            if (arg_count == 1)
            { // show the last 15 commmands in pastevents.txt
                execute_pastevents(home_directory);
            }

            else if (arg_count == 2)
            {
                if (strcmp(args[1], "purge") == 0)
                {
                    // remove the file pastevents.txt
                    remove("pastevents.txt");
                }
                else
                {
                    fprintf(stderr, "ERROR: Usage pastevents / pastevents <purge> / pastevents execute <index>\n");
                }
            }

            else if (arg_count == 3)
            {

                if (strcmp(args[1], "execute") == 0)
                {
                    char *command = loadNthLineFromEnd(home_directory, atoi(args[2]) + 1);
                    int h = 0;
                    while (command[h] != '\n')
                    {
                        h++;
                    }
                    command[h] = '\0';

                    // printf("This command will be executed : %s\n", command);
                    // strcpy(input_copy, command);
                    handle_input(command, home_directory, current_directory, last_directory, last_directory_temp, background, previous_command, pepath, input_copy);
                }

                else
                {
                    fprintf(stderr, "ERROR: Usage pastevents / pastevents <purge> / pastevents execute <index>\n");
                }
            }

            else
            {
                fprintf(stderr, "ERROR: Usage pastevents / pastevents <purge> / pastevents execute <index>\n");
            }
            exit(EXIT_SUCCESS);
        }

        else if (strcmp(args[0], "proclore") == 0)
        {
            if (arg_count == 1)
            {
                // obtain the information regarding the current shell
                execute_proclore();
            }

            else if (arg_count == 2)
            {
                // obtain the information regarding the given pid
                execute_proclore_pid(args[1]);
            }

            else
            {
                fprintf(stderr, "ERROR: Usage proclore / proclore <pid>\n");
            }

            exit(EXIT_SUCCESS);
        }

        else if (strcmp(args[0], "iMan") == 0)
        {
            if (arg_count == 2)
            {
                execute_man(args[1]);
            }
            else
            {
                fprintf(stderr, "ERROR: Usage iMan <command>\n");
                return;
            }
            exit(EXIT_SUCCESS);
        }

        else if (strcmp(args[0], "activities") == 0)
        {
            if (arg_count == 1)
            {
                execute_activities();
            }
            else
            {
                fprintf(stderr, "ERROR: Usage activities\n");
                return;
            }
            exit(EXIT_SUCCESS);
        }

        else if (strcmp(args[0], "neonate") == 0)
        {
            if (arg_count != 3)
            {
                fprintf(stderr, "ERROR: Usage neonate -n [time_arg]\n");
                return;
            }

            if (strcmp(args[1], "-n") != 0)
            {
                fprintf(stderr, "ERROR: Usage neonate -n [time_arg]\n");
                return;
            }

            int time_arg = atoi(args[2]);

            execute_neonate(time_arg);

            exit(EXIT_SUCCESS);
        }

        // else if (strcmp(args[0], "fg") == 0)
        // {
        //     if(arg_count != 2)
        //     {
        //         fprintf(stderr, "ERROR: Usage fg <pid>\n");
        //         return;
        //     }

        //     int pid = atoi(args[1]);

        //     execute_fg(pid);

        //     //exit(EXIT_SUCCESS);
        // }

        else if (strcmp(args[0], "bg") == 0)
        {
            if (arg_count != 2)
            {
                fprintf(stderr, "ERROR: Usage bg <pid>\n");
                return;
            }

            int pid = atoi(args[1]);

            execute_bg(pid);

            exit(EXIT_SUCCESS);
        }

        else if (strcmp(args[0], "ping") == 0)
        {
            if (arg_count != 3)
            {
                fprintf(stderr, "ERROR: Usage ping <pid> <signal_number>\n");
                return;
            }

            int pid = atoi(args[1]);
            int signal_number = atoi(args[2]);

            execute_ping(pid, signal_number);

            exit(EXIT_SUCCESS);
        }
        else
        {
            // Execute other commands
            if (execvp(args[0], args) == -1)
            {
                fprintf(stderr, "ERROR: '%s' is not a valid command\n", args[0]);
                exit(EXIT_FAILURE);
            }
        }
        // exit(EXIT_SUCCESS);
    }
    else
    {
        // Parent process
        if (!background)
        {
            int status;
            waitpid(pid, &status, 0);
            // end clock
            gettimeofday(&end, NULL);
            double elapsed_time = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1000000.0;
            // printf("Child process took %.6f seconds.\n", elapsed_time);
            *time = (int)elapsed_time;
            // printf("%d\n", *time);
            normalprocs[normalendidx].pid = pid;
            strcpy(normalprocs[normalendidx].name, args[0]);
            normalendidx++;
            fgpid = -1;
        }
        else
        {
            printf("%d\n", pid);
            bgprocs[endidx].pid = pid;
            strcpy(bgprocs[endidx].name, args[0]);
            endidx++;
            normalprocs[normalendidx].pid = pid;
            strcpy(normalprocs[normalendidx].name, args[0]);
            normalendidx++;
        }
    }
}
