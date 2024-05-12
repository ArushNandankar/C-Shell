#ifndef HEADERS_H_
#define HEADERS_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <grp.h>
#include <time.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <signal.h>
#include <time.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <netdb.h>
#include <arpa/inet.h>


#include "main.h"
#include "prompt.h"
#include "execute.h"
#include "warp.h"
#include "peek.h"
#include "pastevents.h"
#include "proclore.h"
#include "seek.h"
#include "iMan.h"
#include "pipe.h"
#include "ioredirect.h"
#include "activities.h"
#include "neonate.h"
#include "fg_bg.h"
#include "ping.h"
#include "signals.h"

#define COLOR_RESET   "\033[0m"
#define COLOR_BLUE    "\033[34m"
#define COLOR_GREEN   "\033[32m"
#define COLOR_WHITE   "\033[37m"

struct proc
{
    int pid;
    char name[100];
};

extern struct proc bgprocs[100];
extern int startidx;
extern int endidx;

extern struct proc normalprocs[1000];
extern int normalstartidx;
extern int normalendidx;
extern int fgpid;

#endif
