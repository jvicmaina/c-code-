#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>

// Declaration of empty environment variable array
char *empty_env[] = {NULL};  

#define SERVERS_DIR "/workspaces/hw4/daemons"
#define CHILD_TIMEOUT 10 // Timeout duration in seconds
#define MAX_INPUT_LENGTH 256
#define MAX_DAEMONS 10
#define MAX_DAEMON_NAME_LENGTH 20
#define MAX_EXECUTABLE_NAME_LENGTH 50
#define SYNC_FD 3 // File descriptor for synchronization

// Struct to represent a registered daemon
typedef struct {
    char name[MAX_DAEMON_NAME_LENGTH];
    char executable[MAX_EXECUTABLE_NAME_LENGTH];
    pid_t pid;
    int status; // 0: inactive, 1: starting, 2: active, 3: stopping, 4: exited, 5: crashed
} Daemon;

// Array to store registered daemons
Daemon registered_daemons[MAX_DAEMONS];
int num_registered_daemons = 0;



// Enum to represent daemon status
enum DaemonStatus {
    UNKNOWN,
    INACTIVE,
    STARTING,
    ACTIVE,
    STOPPING,
    EXITED,
    CRASHED
};

// Function to print the help message
void print_help(FILE *output_stream) {
    fprintf(output_stream, "Available commands:\n");
    fprintf(output_stream, "help (0 args) Print this help message\n");
    fprintf(output_stream, "quit (0 args) Quit the program\n");
    fprintf(output_stream, "register (0 args) Register a daemon\n");
    fprintf(output_stream, "unregister (1 args) Unregister a daemon\n");
    fprintf(output_stream, "status (1 args) Show the status of a daemon\n");
    fprintf(output_stream, "status-all (0 args) Show the status of all daemons\n");
    fprintf(output_stream, "start (1 args) Start a daemon\n");
    fprintf(output_stream, "stop (1 args) Stop a daemon\n");
    fprintf(output_stream, "logrotate (1 args) Rotate log files for a daemon\n");
}

// Function to register a daemon
void register_daemon(const char *daemon_name, const char *executable_name, FILE *output_stream) {
    // Check if maximum number of daemons already registered
    if (num_registered_daemons >= MAX_DAEMONS) {
        fprintf(output_stream, "Error: Maximum number of daemons registered\n");
        return;
    }
    // Check if daemon name is already registered
    for (int i = 0; i < num_registered_daemons; i++) {
        if (strcmp(registered_daemons[i].name, daemon_name) == 0) {
            fprintf(output_stream, "Error: Daemon '%s' is already registered\n", daemon_name);
            return;
        }
    }
    // Register the daemon
    strcpy(registered_daemons[num_registered_daemons].name, daemon_name);
    strcpy(registered_daemons[num_registered_daemons].executable, executable_name);
    registered_daemons[num_registered_daemons].status = INACTIVE;
    fprintf(output_stream, "Registered daemon '%s' with command '%s'\n", daemon_name, executable_name);
    num_registered_daemons++;
}

// Function to unregister a daemon
void unregister_daemon(const char *daemon_name, FILE *output_stream) {
    int i;
    // Search for the daemon in the registered daemons array
    for (i = 0; i < num_registered_daemons; i++) {
        if (strcmp(registered_daemons[i].name, daemon_name) == 0) {
            // Daemon found, remove it from the array
            fprintf(output_stream, "Unregistered daemon '%s'\n", daemon_name);
            for (; i < num_registered_daemons - 1; i++) {
                registered_daemons[i] = registered_daemons[i + 1];
            }
            num_registered_daemons--;
            return;
        }
    }
    // Daemon not found, display an error message
    fprintf(output_stream, "Error: Daemon '%s' is not registered\n", daemon_name);
}

// Function to handle the status command
void print_daemon_status(const char *daemon_name, FILE *output_stream) {
    for (int i = 0; i < num_registered_daemons; i++) {
        if (strcmp(registered_daemons[i].name, daemon_name) == 0) {
            // Daemon found
            switch (registered_daemons[i].status) {
                case INACTIVE:
                    fprintf(output_stream, "%s\t0\tinactive\n", daemon_name);
                    break;
                case STARTING:
                case ACTIVE:
                case STOPPING:
                    fprintf(output_stream, "%s\t%d\t%s\n", daemon_name, registered_daemons[i].pid,
                            registered_daemons[i].status == STARTING ? "starting" :
                            registered_daemons[i].status == ACTIVE ? "active" : "stopping");
                    break;
                case EXITED:
                    fprintf(output_stream, "%s\t%d\texited\n", daemon_name, registered_daemons[i].pid);
                    break;
                case CRASHED:
                    fprintf(output_stream, "%s\t%d\tcrashed\n", daemon_name, registered_daemons[i].pid);
                    break;
                default:
                    fprintf(output_stream, "%s\t0\tunknown\n", daemon_name);
            }
            return;
        }
    }
    // Daemon not found
    fprintf(output_stream, "%s\t0\tunknown\n", daemon_name);
}

// Function to start a daemon
void start_daemon(const char *daemon_name, FILE *output_stream) {
    // Step 1: Verify if the daemon is registered
    int daemon_index = -1;
    for (int i = 0; i < num_registered_daemons; i++) {
        if (strcmp(registered_daemons[i].name, daemon_name) == 0) {
            daemon_index = i;
            break;
        }
    }

    if (daemon_index == -1) {
        fprintf(output_stream, "Error: Daemon '%s' is not registered\n", daemon_name);
        return;
    }

    // Step 2: Check if the daemon is inactive
    if (registered_daemons[daemon_index].status != INACTIVE) {
        fprintf(output_stream, "Error: Daemon '%s' is not inactive\n", daemon_name);
        return;
    }

    // Step 3: Set the status of the daemon to starting
    registered_daemons[daemon_index].status = STARTING;

    // Step 4: Fork a child process
    pid_t pid = fork();
    if (pid < 0) {
        // Fork failed
        perror("Fork failed");
        registered_daemons[daemon_index].status = INACTIVE; // Reset status
        return;
    } else if (pid == 0) {
        // Child process
        // Proceed with the remaining steps for the child process
    } else {
        // Parent process
        // Proceed with the remaining steps for the parent process
    }

// Step 6: Redirect standard output of the child process to the appropriate log file
        char log_file_name[50]; // Assuming maximum length of log file name is 50 characters
        sprintf(log_file_name, "%s.log", registered_daemons[daemon_index].name); // Create log file name
        FILE *log_file = fopen(log_file_name, "a"); // Open log file in append mode
        if (log_file == NULL) {
            perror("Failed to open log file");
            exit(EXIT_FAILURE); // Exit child process
        }
        dup2(fileno(log_file), STDOUT_FILENO); // Redirect stdout to log file
        fclose(log_file); // Close the file stream after redirection   

// Step 7: Execute the command registered for the daemon using execvpe()
        char *argv[] = {registered_daemons[daemon_index].executable, NULL}; // Command and arguments
        execvpe(argv[0], argv, empty_env); // Execute command
        // If execvpe() returns, it failed to execute the command
        perror("Exec failed");
        exit(EXIT_FAILURE);  
}


// Function to handle command-line interface
void run_cli(FILE *input_stream, FILE *output_stream) {
    char input[MAX_INPUT_LENGTH];
    char *token;
    while (1) {
        fprintf(output_stream, "legion> ");
        if (fgets(input, sizeof(input), input_stream) == NULL) {
            fprintf(output_stream, "\n");
            break;
        }
        // Tokenize input and process command
        token = strtok(input, " \n");
        if (token != NULL) {
            // Compare the command token with known commands
            if (strcmp(token, "help") == 0) {
                print_help(output_stream);
            } else if (strcmp(token, "quit") == 0) {
                fprintf(output_stream, "Exiting program...\n");
                break; // Exit the loop and the function
            } else if (strcmp(token, "register") == 0) {
                // Handle register command
                char *daemon_name = strtok(NULL, " \n");
                char *executable_name = strtok(NULL, " \n");
                if (daemon_name != NULL && executable_name != NULL) {
                    register_daemon(daemon_name, executable_name, output_stream);
                } else {
                    fprintf(output_stream, "Usage: register <daemon_name> <executable_name>\n");
                }
            } else if (strcmp(token, "unregister") == 0) {
                // Handle unregister command
                char *daemon_name = strtok(NULL, " \n");
                if (daemon_name != NULL) {
                    unregister_daemon(daemon_name, output_stream);
                } else {
                    fprintf(output_stream, "Usage: unregister <daemon_name>\n");
                }
            } else if (strcmp(token, "status") == 0) {
                // Handle status command
                char *daemon_name = strtok(NULL, " \n");
                if (daemon_name != NULL) {
                    print_daemon_status(daemon_name, output_stream);
                } else {
                    fprintf(output_stream, "Usage: status <daemon_name>\n");
                }
            } else if (strcmp(token, "status-all") == 0) {
                // Handle status-all command
                for (int i = 0; i < num_registered_daemons; i++) {
                    print_daemon_status(registered_daemons[i].name, output_stream);
                }
            } else if (strcmp(token, "start") == 0) {
                // Handle start command
                char *daemon_name = strtok(NULL, " \n");
                if (daemon_name != NULL) {
                    start_daemon(daemon_name, output_stream);
                } else {
                    fprintf(output_stream, "Usage: start <daemon_name>\n");
                }
            } else {
                fprintf(output_stream, "Unknown command: %s\n", token);
            }
        }
    }
}


