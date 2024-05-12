#ifndef _WARP_H_
#define _WARP_H_

void change_directory(const char *path);
void execute_warp(const char *arg, char *home_directory, char *last_directory);

#endif

// void handle_input(char *input, char *home_directory, char *current_directory, char *last_directory, char *last_directory_temp)
// {
//     char *token = strtok(input, ";");
//     int background;
//     while (token != NULL)
//     {
//         if (token[strlen(token) - 1] == '&')
//         {
//             background = 1;
//             token[strlen(token) - 1] = '\0';
//         }
//         // Execute the command
//         // printf("token: %s\n", token);
//         execute_command(token, background, home_directory, current_directory, last_directory, last_directory_temp);
//         token = strtok(NULL, ";");
//     }
// }