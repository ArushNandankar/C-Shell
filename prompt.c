#include "headers.h"

// extern char home_directory[256];
// extern char current_directory[256];
// extern char last_directory[256];
// extern char last_directory_temp[256];

void prompt(char *home_directory, char *current_directory, char *string)
{
    // Do not hardcode the prmopt
    // printf("<Everything is a file> ");

    // Get the user ID of the current user
    uid_t user_id = getuid();

    // Get the password entry for the user ID
    struct passwd *pw_entry = getpwuid(user_id);

    if (pw_entry != NULL)
    {
        // printf("Username: %s\n", pw_entry->pw_name);
    }
    else
    {
        perror("Failed to get username");
    }

    char hostname[256]; // Allocate space for the hostname

    if (gethostname(hostname, sizeof(hostname)) == 0)
    {
        // printf("Hostname: %s\n", hostname);
    }
    else
    {
        perror("Failed to get hostname");
    }

    if (getcwd(current_directory, sizeof(char) * 256) != NULL)
    {
        // printf("Current dir: %s\n", current_directory);
        // printf("Last dir: %s\n", last_directory);
    }
    else
    {
        printf("prompt.c line 70\n");
        perror("getcwd() error");
    }

    if (strlen(home_directory) <= strlen(current_directory))
    {
        int i = 0;
        while (home_directory[i] != '\0' && home_directory[i] == current_directory[i])
        {
            i++;
        }

        if (home_directory[i] == '\0' && (current_directory[i] == '\0' || current_directory[i] == '/'))
        {
            current_directory[0] = '~';
            int j = 1;

            while (current_directory[i] != '\0')
            {
                current_directory[j] = current_directory[i];
                i++;
                j++;
            }

            current_directory[j] = '\0';
        }
    }
    printf("\033[1;34m<%s@%s:%s %s> \033[0m", pw_entry->pw_name, hostname, current_directory, string);
}
