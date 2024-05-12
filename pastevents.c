#include "headers.h"

// function to print last 15 commands from pastevents.txt
void execute_pastevents(char *homedirectory)
{
    char *path = malloc(sizeof(char) * 256);
    sprintf(path, "%s/pastevents.txt", homedirectory);
    int fd = open(path, O_RDONLY);
    if (fd == -1)
    {
        perror("Error opening file");
        exit(1);
    }

    // calculate number of lines in file
    int line_count = 0;
    char c;
    while (read(fd, &c, 1) != 0)
    {
        if (c == '\n')
        {
            line_count++;
        }
    }
    close(fd);

    if (line_count < 15)
    {
        fd = open(path, O_RDONLY);
        if (fd == -1)
        {
            perror("Error opening file");
            exit(1);
        }
        char buffer[256];
        int bytes_read = read(fd, buffer, 256);
        if (bytes_read == -1)
        {
            perror("Error reading file");
            exit(1);
        }
        write(STDOUT_FILENO, buffer, bytes_read);
        close(fd);
        return;
    }

    fd = open(path, O_RDONLY);
    if (fd == -1)
    {
        perror("Error opening file");
        exit(1);
    }

    off_t file_size = lseek(fd, 0, SEEK_END);
    off_t offset = file_size;
    int lines_printed = 0;
    char buffer[256];
    int newline_count = 0;

    while (offset > 0 && lines_printed < 15)
    {
        off_t read_size = (offset > 256) ? 256 : offset;
        offset -= read_size;

        lseek(fd, offset, SEEK_SET);
        ssize_t bytes_read = read(fd, buffer, read_size);
        if (bytes_read == -1)
        {
            perror("Error reading file");
            exit(1);
        }

        for (ssize_t i = bytes_read - 1; i >= 0; i--)
        {
            if (buffer[i] == '\n')
            {
                newline_count++;
                if (newline_count >= 15)
                {
                    write(STDOUT_FILENO, buffer + i + 1, bytes_read - i - 1);
                    lines_printed = 15;
                    break;
                }
            }
        }
    }
    close(fd);
}

char *loadNthLineFromEnd(const char *homedirectory, int n)
{
    char filename[256];
    sprintf(filename, "%s/pastevents.txt", homedirectory);
    int fd = open(filename, O_RDONLY);

    if (fd == -1) {
        perror("Error opening file");
        return NULL;
    }

    off_t file_size = lseek(fd, 0, SEEK_END);
    if (file_size == -1) {
        perror("Error getting file size");
        close(fd);
        return NULL;
    }

    char* line = (char*)malloc(65536);
    if (line == NULL) {
        perror("Error allocating memory");
        close(fd);
        return NULL;
    }

    off_t offset = file_size - 1;
    int newline_count = 0;

    // Skip any empty lines at the end of the file
    while (offset >= 0) {
        char c;
        lseek(fd, offset, SEEK_SET);
        ssize_t bytes_read = read(fd, &c, 1);

        if (bytes_read == -1) {
            perror("Error reading file");
            free(line);
            close(fd);
            return NULL;
        }

        if (c == '\n') {
            newline_count++;
            if (newline_count >= n) {
                break;
            }
        }

        offset--;
    }

    ssize_t bytes_read = read(fd, line, 65536 - 1);
    if (bytes_read == -1) {
        perror("Error reading file");
        free(line);
        close(fd);
        return NULL;
    }

    line[bytes_read] = '\0'; // Null-terminate the string

    close(fd);
    return line;
}