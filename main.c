#include "headers.h"

char pepathtemp[256];
char homedirtemp[256];
char promptS[256];

int timep;

struct proc bgprocs[100];
int startidx = 0;
int endidx = 0;

struct proc normalprocs[1000];
int normalstartidx = 0;
int normalendidx = 0;
int fgpid = -1;

int main()
{
    // Set up signal handlers
    signal(SIGINT, sigint_handler);   // Ctrl+C (SIGINT)
    signal(SIGTSTP, sigtstp_handler); // Ctrl+Z (SIGTSTP)


    // pid_t pid = getpid();
    // printf("pid of shell: %d\n", pid);
    strcpy(promptS, "");

    timep = 0;

    char home_directory[256];
    char previous_command[256];
    char current_directory[256];
    char last_directory[256];
    char last_directory_temp[256];
    char lastinput[256] = "";

    char promptS[256] = "";

    // Keep accepting commands
    if (getcwd(home_directory, sizeof(char) * 256) != NULL)
    {
        // printf("Current working dir: %s\n", cwd);
    }
    else
    {
        printf("main.c line 20\n");
        perror("getcwd() error");
    }

    strcpy(last_directory, home_directory);
    strcpy(homedirtemp, home_directory);

    char *pepath = malloc(sizeof(char) * 256);
    sprintf(pepath, "%s/pastevents.txt", home_directory);

    strcpy(pepathtemp, pepath);

    remove_empty_lines(pepath, home_directory);
    removeConsecutiveDuplicates(pepath, home_directory);

    while (1)
    {
        // Print appropriate prompt with username, system name, and directory before accepting input

        int idx = 0;
        while (1)
        {
            if (lastinput[idx] == '\0')
            {
                break;
            }
            else if (lastinput[idx] == '\n')
            {
                lastinput[idx] = '\0';
                break;
            }
            idx++;
        }

        if (timep > 2)
        {
            sprintf(promptS, "%s : %ds", lastinput, timep);
            timep = 0;
        }
        else
        {
            sprintf(promptS, "");
            timep = 0;
        }

        prompt(home_directory, current_directory, promptS);
        char input[4096];
        char *ptr = fgets(input, sizeof(input), stdin);

        if(ptr == NULL)
        {
            eof_handler(0);
        }

        check_bg_process();

        // modify_input(input);
        char input_copy[4096];
        strcpy(input_copy, input);
        strcpy(lastinput, input);
        // Tokenize input by '&' to separate background commands
        // char *background_token = strtok(input, "&");
        // while (background_token != NULL)
        //{

        // Check if the command should run in the background
        int background = 0;

        // Tokenize background_token by ';'

        int pastevents = open(pepath, O_RDONLY);
        if (pastevents == -1)
        {
            perror("Error opening file");
            exit(1);
        }

        off_t file_size = lseek(pastevents, 0, SEEK_END);
        off_t offset = file_size;
        char buffer[256];
        int newline_count = 0;

        // Read the last line, considering edge cases
        while (offset > 0)
        {
            char c;
            lseek(pastevents, --offset, SEEK_SET);
            read(pastevents, &c, 1);

            if (c == '\n')
            {
                if (newline_count == 1)
                {
                    // Reached the end of the last line
                    offset++;
                    break;
                }
                newline_count++;
            }
        }

        int bytes_read = read(pastevents, buffer, sizeof(buffer) - 1);
        if (bytes_read == -1)
        {
            perror("Error reading file");
            exit(1);
        }
        buffer[bytes_read] = '\0'; // Null-terminate the string

        close(pastevents);
        strcpy(previous_command, buffer);
        // printf("previous_command: %s\n", previous_command);

        // char *token = strtok(input, ";");
        // while (token != NULL)
        // {
        //     if (token[strlen(token) - 1] == '&')
        //     {
        //         background = 1;
        //         token[strlen(token) - 1] = '\0';
        //     }
        //     // Execute the command
        //     // printf("token: %s\n", token);
        //     execute_command(token, background, home_directory, current_directory, last_directory, last_directory_temp);
        //     token = strtok(NULL, ";");
        // }

        handle_input(input, home_directory, current_directory, last_directory, last_directory_temp, background, previous_command, pepath, input_copy);

        // printf("input_copy %s\n", input);
    }
}

// remove empty lines from pastevents.txt
void remove_empty_lines(char *pepath, char *home_directory)
{
    char *tempath = malloc(sizeof(char) * 256);
    sprintf(tempath, "%s/temp.txt", home_directory);
    int pastevents = open(pepath, O_RDWR | O_APPEND | O_CREAT, 0644);
    int temp = open(tempath, O_RDWR | O_APPEND | O_CREAT, 0644);
    char ch;
    int flag = 0;
    while (read(pastevents, &ch, 1))
    {
        if (ch == '\n')
        {
            if (flag == 0)
            {
                write(temp, &ch, 1);
                flag = 1;
            }
        }
        else
        {
            write(temp, &ch, 1);
            flag = 0;
        }
    }
    remove(pepath);
    rename(tempath, pepath);
    close(pastevents);
    close(temp);
}

int isSubstring(const char *str, const char *substr)
{
    int str_len = strlen(str);
    int substr_len = strlen(substr);

    for (int i = 0; i <= str_len - substr_len; i++)
    {
        int j;

        // Check if current substring of str matches the given substring
        for (j = 0; j < substr_len; j++)
        {
            if (str[i + j] != substr[j])
            {
                break;
            }
        }

        if (j == substr_len)
        {
            return 1; // Substring found
        }
    }

    return 0; // Substring not found
}

void handle_input(char *input, char *home_directory, char *current_directory, char *last_directory, char *last_directory_temp, int background, char *previous_command, char *pepath, char *input_copy)
{
    // printf("input: %s\n", input);
    strcpy(input_copy, input);
    modify_input(input);
    // input = strtok(input, "\n");
    // printf("input after modify: %s\n", input);
    char *saveptr;
    char *token = strtok_r(input, ";", &saveptr);
    while (token != NULL)
    {

        // printf("token: %s\n", token);

        // Execute the command
        // printf("token: %s\n", token);

        // if token contains >, >>, < or |

        int pipeflag = 0;
        int idx = 0;
        while (token[idx] != '\0')
        {
            if (token[idx] == '|')
            {
                pipeflag = 1;
                execute_command_haspipes(token, background, home_directory, current_directory, last_directory, last_directory_temp, previous_command, pepath, input_copy, &timep);
                break;
            }
            idx++;
        }

        int ioflag = 0;
        if (!pipeflag)
        {
            idx = 0;
            while (token[idx] != '\0')
            {
                if (token[idx] == '>' || token[idx] == '<')
                {
                    ioflag = 1;
                    execute_command_hasio(token, background, home_directory, current_directory, last_directory, last_directory_temp, previous_command, pepath, input_copy, &timep);
                    break;
                }
                idx++;
            }
        }

        if (!pipeflag && !ioflag)
            execute_command(token, background, home_directory, current_directory, last_directory, last_directory_temp, previous_command, pepath, input_copy, &timep);

        token = strtok_r(NULL, ";", &saveptr);
    }

    int pastevents = open(pepath, O_RDWR | O_APPEND | O_CREAT, 0644);
    if (strcmp(input_copy, previous_command) != 0 && isSubstring(input_copy, "pastevents") == 0)
    {
        write(pastevents, input_copy, strlen(input_copy));
        write(pastevents, "\n", 1);
    }
    // strcpy(previous_command, token);
    close(pastevents);

    // remove empty lines from pastevents.txt
    remove_empty_lines(pepath, home_directory);
    removeConsecutiveDuplicates(pepath, home_directory);
}

// void handle_input(char *input, char *home_directory, char *current_directory, char *last_directory, char *last_directory_temp, int background, char *previous_command, char *pepath, char *input_copy)
// {
//     // printf("Input is %s\n", input);
//     strcpy(input_copy, input);
//     // printf("input: %s\n", input);
//     modify_input(input);
//     char *saveptr;
//     char *token = strtok_r(input, ";", &saveptr);
//     while (token != NULL)
//     {
//         // Execute the command
//         // printf("token: %s\n", token);

//         // if token contains >, >> , < or |
//         int pipeflag = 0;
//         int idx = 0;
//         while (token[idx] != '\0')
//         {
//             if (token[idx] == '|')
//             {
//                 pipeflag = 1;
//                 execute_command_haspipes(token, background, home_directory, current_directory, last_directory, last_directory_temp, previous_command, pepath, input_copy, &timep);
//                 break;
//             }
//             idx++;
//         }

//         int ioflag = 0;
//         if (!pipeflag)
//         {
//             idx = 0;
//             while (token[idx] != '\0')
//             {
//                 if (token[idx] == '>' || token[idx] == '<')
//                 {
//                     ioflag = 1;
//                     execute_command_hasio(token, background, home_directory, current_directory, last_directory, last_directory_temp, previous_command, pepath, input_copy, &timep);
//                     break;
//                 }
//                 idx++;
//             }
//         }

//         if (!pipeflag && !ioflag)
//             execute_command(token, background, home_directory, current_directory, last_directory, last_directory_temp, previous_command, pepath, input_copy, &timep);
//         token = strtok_r(NULL, ";", &saveptr);
//     }

//     int pastevents = open(pepath, O_RDWR | O_APPEND | O_CREAT, 0644);
//     if (strcmp(input_copy, previous_command) != 0 && isSubstring(input_copy, "pastevents") == 0)
//     {
//         write(pastevents, input_copy, strlen(input_copy));
//         write(pastevents, "\n", 1);
//     }
//     // strcpy(previous_command, token);
//     close(pastevents);

//     // remove empty lines from pastevents.txt
//     remove_empty_lines(pepath, home_directory);
//     removeConsecutiveDuplicates(pepath, home_directory);
// }

void removeConsecutiveDuplicates(const char *pepath, const char *home_directory)
{
    char *inputFilename = malloc(sizeof(char) * 256);
    sprintf(inputFilename, "%s", pepath);
    char *outputFilename = malloc(sizeof(char) * 256);
    sprintf(outputFilename, "%s/%s", home_directory, "temp.txt");
    int inputFd = open(inputFilename, O_RDONLY);
    int outputFd = open(outputFilename, O_WRONLY | O_CREAT | O_TRUNC, 0644);

    if (inputFd == -1 || outputFd == -1)
    {
        perror("Error opening files");
        return;
    }

    char previousLine[8192] = ""; // Assuming a maximum line length of 8192 characters
    char currentLine[8192];
    ssize_t bytesRead;

    while ((bytesRead = read(inputFd, currentLine, sizeof(currentLine))) > 0)
    {
        char *lineStart = currentLine;
        char *lineEnd = currentLine + bytesRead;
        char *newlinePos;

        while ((newlinePos = memchr(lineStart, '\n', lineEnd - lineStart)) != NULL)
        {
            *newlinePos = '\0'; // Null-terminate the line

            if (strcmp(lineStart, previousLine) != 0)
            {
                write(outputFd, lineStart, strlen(lineStart));
                write(outputFd, "\n", 1); // Add newline
                strcpy(previousLine, lineStart);
            }

            lineStart = newlinePos + 1;
        }

        // Copy any remaining partial line to previousLine for comparison with the next batch
        strcpy(previousLine, lineStart);
    }

    remove(inputFilename);
    rename(outputFilename, inputFilename);

    close(inputFd);
    close(outputFd);
}

void modify_input(char *input)
{
    if(input[0] == '\n')
    {
        input[0] = '\0';
        return;
    }

    // replace all "&" in the string the replace it by "&;"
    char temp[4096];
    int idx1 = 0;
    int idx2 = 0;
    while (input[idx1] != '\0')
    {
        if (input[idx1] == '&')
        {
            temp[idx2] = '&';
            temp[idx2 + 1] = ';';
            idx2 += 2;
            idx1++;
        }
        else
        {
            temp[idx2] = input[idx1];
            idx1++;
            idx2++;
        }
    }
    strcpy(input, temp);
    input = strtok(input, "\n");
}

int readline(int fd, char *buffer)
{
    int idx = 0;
    char ch;
    while (read(fd, &ch, 1))
    {
        if (ch == '\n')
        {
            buffer[idx] = '\0';
            return 1;
        }
        buffer[idx] = ch;
        idx++;
    }
    return -1;
}

void remove_line(int fd, char *line)
{
    char *tempath = malloc(sizeof(char) * 256);
    sprintf(tempath, "%s/temp.txt", homedirtemp);
    int temp = open(tempath, O_RDWR | O_APPEND | O_CREAT, 0644);
    char *line_copy = malloc(sizeof(char) * 256);
    strcpy(line_copy, line);
    char *line_token = strtok(line_copy, " ");
    int pid = atoi(line_token);
    char *saveptr;
    char *token = strtok_r(line, " ", &saveptr);
    while (readline(fd, line) != -1)
    {
        char *token = strtok_r(line, " ", &saveptr);
        int pid2 = atoi(token);
        if (pid != pid2)
        {
            write(temp, line, strlen(line));
            write(temp, "\n", 1);
        }
    }
    remove("background.txt");
    rename(tempath, "background.txt");
    close(temp);
}

void check_bg_process()
{
    for (int i = startidx; i < endidx; i++)
    {
        int pid = bgprocs[i].pid;
        int status;
        // if process has ended normally
        if (waitpid(pid, &status, WNOHANG) == pid)
        {
            printf("%s exited normally (%d)\n", bgprocs[i].name, pid);
            for (int j = i; j < endidx - 1; j++)
            {
                bgprocs[j] = bgprocs[j + 1];
            }
            endidx--;
            i--;
        }
        // if the process ended abnormally
        else if (waitpid(pid, &status, WNOHANG) == -1)
        {
            printf("%s exited abnormally (%d)\n", bgprocs[i].name, pid);
            for (int j = i; j < endidx - 1; j++)
            {
                bgprocs[j] = bgprocs[j + 1];
            }
            endidx--;
            i--;
        }
    } 
}