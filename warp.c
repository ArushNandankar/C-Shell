#include "headers.h"

// extern char home_directory[256];
// extern char current_directory[256];
// extern char last_directory[256];
// extern char last_directory_temp[256];

void change_directory(const char *path)
{
    char new_path[256];

    if (chdir(path) == 0)
    {
        getcwd(new_path, sizeof(new_path));
        printf("%s\n", new_path);
        //strcpy(last_directory, current_directory);
    }
    else
    {
        fprintf(stderr, "chdir failed\n");
    }
}

void execute_warp(const char *arg, char *home_directory, char *last_directory)
{
    if (arg == NULL)
    {
        // No argument provided, warp to home directory
        change_directory(home_directory);
    }
    else
    {
        char *token;
        char copy_arg[256];
        strcpy(copy_arg, arg);

        // Tokenize the argument by spaces
        token = strtok(copy_arg, " \t\n");
        while (token != NULL)
        {
            if (strcmp(token, "-") == 0)
            {
                // Go back to the previous directory
                change_directory(last_directory);
            }
            else if (strcmp(token, "~") == 0)
            {
                // Go to the home directory
                change_directory(home_directory); // done
            }
            else if (strcmp(token, "..") == 0)
            {
                // Go up one level in the directory hierarchy
                change_directory("..");
            }
            else
            {
                // Change to the specified directory
                change_directory(token);
            }

            token = strtok(NULL, " \t\n");
        }
    }
}