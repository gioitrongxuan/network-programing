#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define LOGIN_TYPE "LOGIN"
#define TEXT_TYPE "TEXT"
#define BUFFER_SIZE 1024


int main(int argc, char *argv[]) {
    if (argc != 2)
    {
        printf("Usage: ./server <port_number>\n");
        return 0;
    }
    int PORT = atoi(argv[1]);

    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[BUFFER_SIZE] = {0};
    char clients[10][50]; // Store login names (maximum 10 clients for simplicity)
    FILE *log_files[10] = {NULL}; // Store file pointers for logs

    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 3) < 0) {
        perror("listen failed");
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d\n", PORT);

    while (1) {
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0) {
            perror("accept failed");
            exit(EXIT_FAILURE);
        }

        printf("Connection established with client\n");

        while (1) {
            memset(buffer, 0, BUFFER_SIZE);
            int valread = read(new_socket, buffer, BUFFER_SIZE);
            if (valread <= 0) {
                printf("Client disconnected\n");
                close(new_socket);
                break;
            }

            char *message_type = strtok(buffer, " ");
            char *content = strtok(NULL, "\n");

            if (strcmp(message_type, LOGIN_TYPE) == 0) {
                int client_index = new_socket % 10; // Simple hash for indexing
                strcpy(clients[client_index], content);
                char filename[100];
                sprintf(filename, "%s_log.txt", content);
                log_files[client_index] = fopen(filename, "a");
                if (log_files[client_index] == NULL) {
                    perror("Error opening log file");
                }
                send(new_socket, "Login successful\n", strlen("Login successful\n"), 0);
            } else if (strcmp(message_type, TEXT_TYPE) == 0) {
                int client_index = new_socket % 10;
                if (log_files[client_index] != NULL) {
                    fprintf(log_files[client_index], "%s\n", content);
                    fflush(log_files[client_index]);
                    send(new_socket, "Message logged\n", strlen("Message logged\n"), 0);
                } else {
                    send(new_socket, "Please login first\n", strlen("Please login first\n"), 0);
                }
            } else {
                send(new_socket, "Invalid message type\n", strlen("Invalid message type\n"), 0);
            }
        }
    }
}
