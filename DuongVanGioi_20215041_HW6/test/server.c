#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define MAX_CLIENTS 100
#define BUFFER_SIZE 1024

void handle_client(int client_socket);

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s PortNumber\n", argv[0]);
        return 1;
    }

    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_addr_len;
    int port = atoi(argv[1]);

    // Create socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("Could not create socket");
        return 1;
    }

    // Bind socket
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);
    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        close(server_socket);
        return 1;
    }

    // Listen for connections
    listen(server_socket, MAX_CLIENTS);
    printf("Server listening on port %d\n", port);

    while (1) {
        // Accept client connection
        client_addr_len = sizeof(client_addr);
        client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_addr_len);
        if (client_socket < 0) {
            perror("Accept failed");
            continue;
        }

        // Fork a process for each client
        if (fork() == 0) {
            close(server_socket);  // Close the server socket in child process
            handle_client(client_socket);
            close(client_socket);
            exit(0);
        }
        close(client_socket);
    }

    close(server_socket);
    return 0;
}

void handle_client(int client_socket) {
    char buffer[BUFFER_SIZE];
    int read_size;
    // Handle login and logout functionality here
    // Check credentials, limit attempts, and lock accounts if needed
    // Communicate with the client based on their input
}
