#include "headers.h"

// extern char home_directory[256];
// extern char current_directory[256];
// extern char last_directory[256];
// extern char last_directory_temp[256];

void print_entry(const char *name, mode_t mode) {
    if (S_ISDIR(mode)) {
        printf(COLOR_BLUE "%s\n" COLOR_RESET, name);
        // printf(" <<Directory>>\n");
    }  else if (mode & S_IXUSR || mode & S_IXGRP || mode & S_IXOTH) {
        printf(COLOR_GREEN "%s\n" COLOR_RESET, name);
        // printf(" <<Executable>>\n");
    } else {
        printf(COLOR_WHITE "%s\n" COLOR_RESET, name);
        // printf(" <<File>>\n");
    }
}

void print_info(const char *path, const char *name, mode_t mode) {
    struct stat fileStat;
    if (stat(path, &fileStat) < 0) {
        perror("stat");
        return;
    }

    struct passwd *pw = getpwuid(fileStat.st_uid);
    struct group *gr = getgrgid(fileStat.st_gid);
    struct tm *timeinfo;
    char time_buf[80];

    timeinfo = localtime(&fileStat.st_mtime);
    strftime(time_buf, sizeof(time_buf), "%b %d %H:%M", timeinfo);

    // printf("Permissions: ");
    printf((S_ISDIR(fileStat.st_mode)) ? "d" : "-");
    printf((fileStat.st_mode & S_IRUSR) ? "r" : "-");
    printf((fileStat.st_mode & S_IWUSR) ? "w" : "-");
    printf((fileStat.st_mode & S_IXUSR) ? "x" : "-");
    printf((fileStat.st_mode & S_IRGRP) ? "r" : "-");
    printf((fileStat.st_mode & S_IWGRP) ? "w" : "-");
    printf((fileStat.st_mode & S_IXGRP) ? "x" : "-");
    printf((fileStat.st_mode & S_IROTH) ? "r" : "-");
    printf((fileStat.st_mode & S_IWOTH) ? "w" : "-");
    printf((fileStat.st_mode & S_IXOTH) ? "x" : "-");
    // printf("\n");

    printf("Links: %hu\t", fileStat.st_nlink);
    printf("Owner: %s\t", pw ? pw->pw_name : "Unknown");
    printf("Group: %s\t", gr ? gr->gr_name : "Unknown");
    printf("%lld bytes\t", fileStat.st_size);
    printf("Modified: %s\t", time_buf);
    
   print_entry(name, mode);
}

void execute_peek(const char *path, int show_hidden, int show_info, char *home_directory, char *last_directory) {
    DIR *dir;
    
    if(strcmp(path, "-") == 0) {
        // printf("about to open %s\n", last_directory);
        //dir = opendir(last_directory);
        strcpy(path, last_directory);
        execute_peek(path, show_hidden, show_info, home_directory, last_directory);
        return;
    } else if(strcmp(path, "~") == 0) {
        // printf("about to open %s\n", home_directory);
        //dir = opendir(home_directory);
        strcpy(path, home_directory);
        execute_peek(path, show_hidden, show_info, home_directory, last_directory);
        return;
    } else {
        // printf("about to open %s\n", path);
        dir = opendir(path);
    }

    if (dir == NULL) {
        perror("opendir");
        return;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (!show_hidden && entry->d_name[0] == '.') {
            continue;
        }

        char full_path[256];
        snprintf(full_path, 256, "%s/%s", path, entry->d_name);

        struct stat file_info;
        if (stat(full_path, &file_info) == -1) {
            perror("stat");
            continue;
        }

        if (show_info) {
            print_info(full_path, entry->d_name, file_info.st_mode);
        } else {
            print_entry(entry->d_name, file_info.st_mode);
        }
    }
    closedir(dir);
}
