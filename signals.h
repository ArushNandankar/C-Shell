#ifndef _SIGS_H_
#define _SIGS_H_

void sigint_handler(int signum);
void eof_handler(int signum);
void sigtstp_handler(int signum);

#endif