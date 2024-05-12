#include "headers.h"

void disableRawMode()
{
    struct termios term;
    tcgetattr(STDIN_FILENO, &term);
    term.c_lflag |= (ECHO | ICANON);
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &term);
}

void enableRawMode()
{
    struct termios term;
    tcgetattr(STDIN_FILENO, &term);
    term.c_lflag &= ~(ECHO | ICANON);
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &term);
}

void execute_neonate(int time_arg)
{
    int stop = 0;

    struct timespec ts;
    ts.tv_sec = time_arg;
    ts.tv_nsec = 0;

    enableRawMode();

    char input[4096];
    int inputidx = 0;

    while (!stop)
    {
        fd_set readfds;
        FD_ZERO(&readfds);
        FD_SET(STDIN_FILENO, &readfds);
        struct timeval timeout;
        timeout.tv_sec = 0;
        timeout.tv_usec = 0;

        if (select(STDIN_FILENO + 1, &readfds, NULL, NULL, &timeout) > 0)
        {
            if (read(STDIN_FILENO, input + inputidx, sizeof(input) - inputidx) > 0)
            {
                inputidx = strlen(input);
                if (strstr(input, "x") != NULL)
                {
                    stop = 1;
                    disableRawMode();
                    exit(0);
                }
            }
        }

        pid_t pid = fork();
        if (pid < 0)
        {
            perror("fork");
            disableRawMode();
            exit(1);
        }
        else if (pid == 0)
        {
            // child process
            printf("%d\n", getpid());
            exit(0);
        }
        nanosleep(&ts, NULL);
    }
}