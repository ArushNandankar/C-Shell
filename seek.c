#include "headers.h"

int matchPattern(const char *input, char *pattern) {
    size_t patternLength = strlen(pattern);
    if (strstr(input, pattern) == input) {
        // The input starts with the pattern
        const char *extension = input + patternLength;
        if (*extension == '\0' || *extension == '.') {
            // The extension is either empty or starts with a dot
            return 1; // Pattern matched
        }
    }
    return 0; // Pattern not matched
}

void execute_seek(char *search, char *dirtosearch, int fileflag, int directoryflag, int eflag, char *home_directory, char *last_directory, int *count, char *onlyfile) {
    if (fileflag == 0 && directoryflag == 0) {
        fileflag = 1;
        directoryflag = 1;
    }

    if (strlen(dirtosearch) == 0) {
        dirtosearch = ".";
    }

    if (dirtosearch[0] == '~') {
        char temp[256] = "\0";
        strcpy(temp, dirtosearch);
        strcpy(dirtosearch, home_directory);
        strcat(dirtosearch, temp + 1);
    }

    if (dirtosearch[0] == '-') {
        char temp[256] = "\0";
        strcpy(temp, dirtosearch);
        strcpy(dirtosearch, last_directory);
        strcat(dirtosearch, temp + 1);
    }

    DIR *dir = opendir(dirtosearch);

    if (dir == NULL) {
        perror("ERROR: Could not open directory\n");
        return;
    }

    // print the relative paths with respect to dirtosearch of all files having prefix search in the entire directory tree of dirtosearch
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_name[0] == '.') {
            continue;
        }

        if (fileflag == 1 && directoryflag == 1) {
            if (matchPattern(entry->d_name, search) == 1) {
                printf("%s/%s\n", dirtosearch, entry->d_name);
                sprintf(onlyfile, "%s/%s", dirtosearch, entry->d_name);
                (*count)++;
            }
        } else if (fileflag == 1) {
            if (matchPattern(entry->d_name, search) == 1) {
                struct stat file_stat;
                char path[256] = "\0";
                strcpy(path, dirtosearch);
                strcat(path, "/");
                strcat(path, entry->d_name);
                if (stat(path, &file_stat) < 0) {
                    perror("ERROR: Could not get file stats\n");
                    return;
                }
                if (S_ISREG(file_stat.st_mode)) {
                    printf("%s/%s\n", dirtosearch, entry->d_name);
                    sprintf(onlyfile, "%s/%s", dirtosearch, entry->d_name);
                    (*count)++;
                }
            }
        } else if (directoryflag == 1) {
            if (matchPattern(entry->d_name, search) == 1) {
                struct stat file_stat;
                char path[256] = "\0";
                strcpy(path, dirtosearch);
                strcat(path, "/");
                strcat(path, entry->d_name);
                if (stat(path, &file_stat) < 0) {
                    perror("ERROR: Could not get file stats\n");
                    return;
                } if (S_ISDIR(file_stat.st_mode)) {
                    printf("./%s\n", entry->d_name);
                    sprintf(onlyfile, "%s/%s", dirtosearch, entry->d_name);
                    (*count)++;
                }
            }
        }
        struct stat file_stat;
        char path[256] = "\0";
        strcpy(path, dirtosearch);
        strcat(path, "/");
        strcat(path, entry->d_name);
        if (stat(path, &file_stat) < 0) {
            perror("ERROR: Could not get file stats\n");
            return;
        }
        if (S_ISDIR(file_stat.st_mode)) {
            execute_seek(search, path, fileflag, directoryflag, eflag, home_directory, last_directory, count, onlyfile);
        }
    }
}
