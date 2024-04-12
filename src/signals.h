#ifndef SIGNALS_H
#define SIGNALS_H

#include <signal.h>

// Signal handler function declarations
void sigint_handler(int signum);
void sigchld_handler(int signum);
void sigalrm_handler(int signum);

// Other declarations...

#endif /* SIGNALS_H */
