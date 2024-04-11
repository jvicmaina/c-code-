#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

int main() {
    // Get current time for logging
    time_t current_time;
    time(&current_time);

    // Print startup message to log
    printf("%ld: LAZY: Starting up\n", current_time);

    // Main loop: Sleep indefinitely
    while (1) {
        // Sleep for 1 second
        sleep(1);
    }

    // This code will never be reached, but included for completeness
    // Print shutdown message to log
    printf("%ld: LAZY: Shutting down\n", current_time);

    return 0;
}
