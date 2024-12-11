#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 1024

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s IPAddress PortNumber\n", argv[0]);
        return 1;
    }

    int client_socket;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];
    char username[50], password[50];

    // Create socket
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1) {
        perror("Could not create socket");
        return 1;
    }

    // Set server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(atoi(argv[2]));
    inet_pton(AF_INET, argv[1], &server_addr.sin_addr);

    // Connect to server
    if (connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection failed");
        close(client_socket);
        return 1;
    }

    // Login prompt
    printf("Username: ");
    scanf("%s", username);
    printf("Password: ");
    scanf("%s", password);

    // Send login details
    snprintf(buffer, BUFFER_SIZE, "%s %s", username, password);
    send(client_socket, buffer, strlen(buffer), 0);

    // Receive response from server
    int read_size = recv(client_socket, buffer, BUFFER_SIZE, 0);
    buffer[read_size] = '\0';
    printf("Server response: %s\n", buffer);

    close(client_socket);
    return 0;
}
