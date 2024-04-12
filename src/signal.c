#include "signals.h"
#include "legion.h"

// Global variables for signal handling
volatile sig_atomic_t sigint_flag = 0;
volatile sig_atomic_t sigchld_flag = 0;
volatile sig_atomic_t sigalrm_flag = 0;

// Signal handler for SIGINT (Ctrl+C)
void sigint_handler(int signum) {
    sigint_flag = 1;
}

// Signal handler for SIGCHLD (child process termination)
void sigchld_handler(int signum) {
    sigchld_flag = 1;
}

// Signal handler for SIGALRM (timeout)
void sigalrm_handler(int signum) {
    sigalrm_flag = 1;
}

// Function to install signal handlers
void install_signal_handlers() {
    // Install signal handlers using sigaction
    // ...
}

// Function to handle signals in the main program
void handle_signals() {
    // Check for and handle signals
    // ...
}
