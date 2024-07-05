#include "headers.h"

void execute_fg(int pid) {
    int status;
    waitpid(pid, &status, WUNTRACED);
    tcsetpgrp(STDIN_FILENO, pid);
    kill(pid, SIGCONT);
    waitpid(pid, &status, WCONTINUED);
    tcsetpgrp(STDIN_FILENO, getpgrp());
}

void execute_bg(int pid) {
    execute_ping(pid, SIGCONT);
}