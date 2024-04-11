#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

int main() {
    // Get current time for logging
    time_t current_time;
    time(&current_time);

    // Print startup message to log
    printf("%ld: YOYO: Starting up\n", current_time);
    fflush(stdout); // Flush the output buffer to ensure the message is printed immediately

    // Sleep for a random interval (between 5 to 10 seconds)
    srand(time(NULL));
    int interval = rand() % 6 + 5; // Random number between 5 and 10
    sleep(interval);

    // Randomly decide to exit or crash
    if (rand() % 2 == 0) {
        // Exiting gracefully
        printf("%ld: YOYO: Exiting\n", current_time);
        fflush(stdout); // Flush the output buffer
        exit(EXIT_SUCCESS);
    } else {
        // Crashing by dereferencing NULL
        int *ptr = NULL;
        *ptr = 42; // This will cause a segmentation fault
    }

    return 0; // This code will never be reached
}
