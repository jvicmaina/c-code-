#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <signal.h> // Include for signal handling
#include "legion.h"

// Global variables for signal handling
volatile sig_atomic_t sigchld_received = 0;
volatile sig_atomic_t sigint_received = 0;
volatile sig_atomic_t sigalrm_received = 0;

// Define sf_manual_mode as global variable
int sf_manual_mode = 0;

// Function prototypes for signal handlers
void sigchld_handler(int signum);
void sigint_handler(int signum);
void sigalrm_handler(int signum);
void install_signal_handlers(void);

// Function implementations

void sigchld_handler(int signum) {
    sigchld_received = 1;
}

void sigint_handler(int signum) {
    sigint_received = 1;
}

void sigalrm_handler(int signum) {
    sigalrm_received = 1;
}

void install_signal_handlers(void) {
    struct sigaction sa;

    // Install SIGCHLD handler
    sa.sa_handler = sigchld_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        perror("sigaction(SIGCHLD) failed");
        exit(EXIT_FAILURE);
    }

    // Install SIGINT handler
    sa.sa_handler = sigint_handler;
    if (sigaction(SIGINT, &sa, NULL) == -1) {
        perror("sigaction(SIGINT) failed");
        exit(EXIT_FAILURE);
    }

    // Install SIGALRM handler
    sa.sa_handler = sigalrm_handler;
    if (sigaction(SIGALRM, &sa, NULL) == -1) {
        perror("sigaction(SIGALRM) failed");
        exit(EXIT_FAILURE);
    }
}

// Helper function to parse user input
void parse_command(char *input, char *command, char **arguments, int *num_arguments) {
    // TO BE IMPLEMENTED
}

// Helper function to execute the command
void execute_command(char *command, char **arguments, int num_arguments) {
    // TO BE IMPLEMENTED
}

// Implementation of run_cli function
void run_cli(FILE *in, FILE *out) {
    // Install signal handlers
    install_signal_handlers();

    char input[1024]; // Buffer to store user input
    char command[128]; // Buffer to store the command
    char *arguments[128]; // Array to store command arguments
    int num_arguments = 0; // Number of command arguments

    while (1) {
        // Print prompt
        fprintf(out, "legion> ");
        fflush(out);

        // Read user input
        if (fgets(input, sizeof(input), in) == NULL) {
            fprintf(out, "\n");
            break; // Exit on EOF
        }

        // Parse command and arguments
        parse_command(input, command, arguments, &num_arguments);

        // Execute command
        execute_command(command, arguments, num_arguments);

        // Reset arguments
        num_arguments = 0;
    }
}
