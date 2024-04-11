#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 10001

int main() {
    // Get current time for logging
    time_t current_time;
    time(&current_time);

    // Print startup message to log
    printf("%ld: FILES: Starting up\n", current_time);
    fflush(stdout); // Flush the output buffer to ensure the message is printed immediately

    // Create socket
    int server_fd;
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Set socket options
    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    // Bind socket
    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // Listen for connections
    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    // Accept incoming connections and serve clients
    while (1) {
        int new_socket;
        int addrlen = sizeof(address);
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
            perror("accept");
            exit(EXIT_FAILURE);
        }

        // Read client input (filename)
        char filename[1024] = {0};
        read(new_socket, filename, sizeof(filename));

        // Open file and send its content to the client
        FILE *file = fopen(filename, "r");
        if (file == NULL) {
            printf("%ld: FILES: Error opening file '%s'\n", current_time, filename);
            fflush(stdout); // Flush the output buffer to ensure the message is printed immediately
            continue;
        }

        // Read file content
        char buffer[1024];
        while (fgets(buffer, sizeof(buffer), file) != NULL) {
            write(new_socket, buffer, sizeof(buffer));
        }

        // Close the file and the connection
        fclose(file);
        close(new_socket);
    }

    return 0; // This code will never be reached
}
