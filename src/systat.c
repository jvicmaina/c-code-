#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

int main() {
    // Get current time for logging
    time_t current_time;
    time(&current_time);

    // Print startup message to log
    printf("%ld: SYSTAT: Starting up\n", current_time);
    fflush(stdout); // Flush the output buffer to ensure the message is printed immediately

    // Main loop: Print system information every 10 seconds
    while (1) {
        // Get system information using the 'uptime' command
        system("uptime");

        // Sleep for 10 seconds
        sleep(10);
    }

    // This code will never be reached, but included for completeness
    // Print shutdown message to log
    printf("%ld: SYSTAT: Shutting down\n", current_time);
    fflush(stdout); // Flush the output buffer

    return 0;
}
