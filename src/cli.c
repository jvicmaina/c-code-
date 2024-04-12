#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <errno.h>

#define MAX_DAEMONS 10
#define MAX_NAME_LENGTH 50
#define LOGFILE_DIR "/var/log/legion"
#define LOG_VERSIONS 10
#define SYNC_FD 3
#define CHILD_TIMEOUT 5

// Daemon status enumeration
enum DaemonStatus {
    UNKNOWN,
    INACTIVE,
    STARTING,
    ACTIVE,
    STOPPING,
    EXITED,
    CRASHED
};

// Daemon structure
typedef struct {
    char name[MAX_NAME_LENGTH];
    pid_t pid;
    enum DaemonStatus status;
} Daemon;

// Array to hold registered daemons
Daemon daemons[MAX_DAEMONS];


// Function to find a free slot in the daemons array
int find_free_slot() {
    for (int i = 0; i < MAX_DAEMONS; i++) {
        if (daemons[i].status == UNKNOWN) {
            return i;
        }
    }
    return -1; // No free slot available
}

// Function to find a daemon by name
Daemon* find_daemon_by_name(char *daemon_name) {
    for (int i = 0; i < MAX_DAEMONS; i++) {
        if (strcmp(daemons[i].name, daemon_name) == 0) {
            return &daemons[i];
        }
    }
    return NULL; // Daemon not found
}

// Function to handle the 'help' command
void help_command() {
    printf("Available commands:\n");
    printf("help (0 args) Print this help message\n");
    printf("quit (0 args) Quit the program\n");
    printf("register (0 args) Register a daemon\n");
    printf("unregister (1 args) Unregister a daemon\n");
    printf("status (1 args) Show the status of a daemon\n");
    printf("status-all (0 args) Show the status of all daemons\n");
    printf("start (1 args) Start a daemon\n");
    printf("stop (1 args) Stop a daemon\n");
    printf("logrotate (1 args) Rotate log files for a daemon\n");
}

// Function to handle the 'logrotate' command
void logrotate_command(char *daemon_name) {
    // Call the function to perform log rotation for the specified daemon
    sf_logrotate(daemon_name);
}

// Function to execute the command
void execute_command(char *command) {
    // Parse command and execute corresponding action
    char *token = strtok(command, " ");
    if (token == NULL) {
        printf("Unknown command: %s\n", command);
        return;
    }

    if (strcmp(token, "help") == 0) {
        // Help command implementation
    } else if (strcmp(token, "quit") == 0) {
        // Quit command implementation
    } else if (strcmp(token, "register") == 0) {
        // Register command implementation
    } else if (strcmp(token, "unregister") == 0) {
        // Unregister command implementation
    } else if (strcmp(token, "status") == 0) {
        // Status command implementation
    } else if (strcmp(token, "status-all") == 0) {
        // Status-all command implementation
    } else if (strcmp(token, "start") == 0) {
        // Start command implementation
    } else if (strcmp(token, "stop") == 0) {
        // Stop command implementation
    } else if (strcmp(token, "logrotate") == 0) {
        // Logrotate command implementation
        char *daemon_name = strtok(NULL, " ");
        if (daemon_name == NULL) {
            printf("Invalid arguments for logrotate command\n");
            return;
        }
        logrotate_command(daemon_name);
    } else {
        printf("Unknown command: %s\n", command);
    }
}


// Function to handle the 'quit' command
void quit_command() {
    // Clean up and exit
    sf_fini();
    exit(0);
}

// Function to handle the 'register' command
void register_command(char *daemon_name, char *executable) {
    // Check if the daemon is already registered
    Daemon *existing_daemon = find_daemon_by_name(daemon_name);
    if (existing_daemon != NULL) {
        sf_error("Daemon already registered");
        return;
    }

    // Find a free slot in the daemons array
    int free_slot = find_free_slot();
    if (free_slot == -1) {
        sf_error("Cannot register more daemons");
        return;
    }

    // Register the daemon
    strncpy(daemons[free_slot].name, daemon_name, MAX_NAME_LENGTH - 1);
    daemons[free_slot].pid = -1;
    daemons[free_slot].status = INACTIVE;
    sf_register(daemon_name);
}

// Function to handle the 'unregister' command
void unregister_command(char *daemon_name) {
    // Find the daemon
    Daemon *daemon = find_daemon_by_name(daemon_name);
    if (daemon == NULL) {
        sf_error("Daemon not found");
        return;
    }

    // Unregister the daemon
    sf_unregister(daemon_name);
    daemon->status = UNKNOWN;
}

// Function to handle the 'status' command
void status_command(char *daemon_name) {
    // Find the daemon
    Daemon *daemon = find_daemon_by_name(daemon_name);
    if (daemon == NULL) {
        sf_error("Daemon not found");
        return;
    }

    // Print the status
    sf_status(daemon_name);
}

// Function to handle the 'status-all' command
void status_all_command() {
    // Print status for all daemons
    for (int i = 0; i < MAX_DAEMONS; i++) {
        if (daemons[i].status != UNKNOWN) {
            sf_status(daemons[i].name);
        }
    }
}

// Function to handle the 'start' command
void start_command(char *daemon_name) {
    // Find the daemon
    Daemon *daemon = find_daemon_by_name(daemon_name);
    if (daemon == NULL) {
        sf_error("Daemon not found");
        return;
    }

    // Check if the daemon is already active
    if (daemon->status == ACTIVE) {
        sf_error("Daemon already active");
        return;
    }

    // Start the daemon
    sf_start(daemon_name);
}

// Function to handle the 'stop' command
void stop_command(char *daemon_name) {
    // Find the daemon
    Daemon *daemon = find_daemon_by_name(daemon_name);
    if (daemon == NULL) {
        sf_error("Daemon not found");
        return;
    }

    // Check if the daemon is already inactive
    if (daemon->status == INACTIVE) {
        sf_error("Daemon already inactive");
        return;
    }

    // Stop the daemon
    sf_stop(daemon_name, daemon->pid);
}

// Function to execute the command
void execute_command(char *command) {
    // Parse command and execute corresponding action
    char *token = strtok(command, " ");
    if (token == NULL) {
        printf("Unknown command: %s\n", command);
        return;
    }

    if (strcmp(token, "help") == 0) {
        help_command();
    } else if (strcmp(token, "quit") == 0) {
        quit_command();
    } else if (strcmp(token, "register") == 0) {
        char *daemon_name = strtok(NULL, " ");
        char *executable = strtok(NULL, " ");
        if (daemon_name == NULL || executable == NULL) {
            printf("Invalid arguments for register command\n");
            return;
        }
        register_command(daemon_name, executable);
    } else if (strcmp(token, "unregister") == 0) {
        char *daemon_name = strtok(NULL, " ");
        if (daemon_name == NULL) {
            printf("Invalid arguments for unregister command\n");
            return;
        }
        unregister_command(daemon_name);
    } else if (strcmp(token, "status") == 0) {
        char *daemon_name = strtok(NULL, " ");
        if (daemon_name == NULL) {
            printf("Invalid arguments for status command\n");
            return;
        }
        status_command(daemon_name);
    } else if (strcmp(token, "status-all") == 0) {
        status_all_command();
    } else if (strcmp(token, "start") == 0) {
        char *daemon_name = strtok(NULL, " ");
        if (daemon_name == NULL) {
            printf("Invalid arguments for start command\n");
            return;
        }
        start_command(daemon_name);
    } else if (strcmp(token, "stop") == 0) {
        char *daemon_name = strtok(NULL, " ");
        if (daemon_name == NULL) {
            printf("Invalid arguments for stop command\n");
            return;
        }
        stop_command(daemon_name);
    } else {
        printf("Unknown command: %s\n", command);
    }
}

void run_cli(FILE *in, FILE *out) {
    char input[256];

    // Enter the main loop
    while (1) {
        // Display prompt
        fprintf(out, "legion> ");
        fflush(out);

        // Read user input
        if (fgets(input, sizeof(input), in) == NULL) {
            fprintf(out, "Error reading input\n");
            continue;
        }

        // Remove newline character
        input[strcspn(input, "\n")] = '\0';

        // Parse input and execute command
        execute_command(input);

        // Check for exit command
        if (strcmp(input, "quit") == 0) {
            break;
        }
    }
}

