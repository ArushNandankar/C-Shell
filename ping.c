#include "headers.h"

void execute_ping(int pid, int signal_number) {
    signal_number = signal_number % 32;
    // send the signal to the process with given pid.
    if(!kill(pid, signal_number)) {
        // printf("Signal sent successfully\n");
    } else {
        printf("No such process exists\n");
    }
}
