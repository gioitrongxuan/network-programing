#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include "network.h"

#define BUFF_SIZE 1024

int main(int argc, char *argv[]) {
    int sockfd, new_socket;
    socklen_t len;
    int addrlen = sizeof(len);
    char buff[BUFF_SIZE + 1];
    struct sockaddr_in servaddr, cliaddr;
    readUsers();
    // Check for command-line arguments
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <port>\n", argv[0]);
        return 1;
    }

    // Parse the port number from command-line argument
    int port = atoi(argv[1]);
    if (port <= 0 || port > 65535) {
        fprintf(stderr, "Error: Invalid port number. Must be between 1 and 65535.\n");
        return 1;
    }

    // Step 1: Construct socket
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Error: ");
        return 1;
    }

    // Step 2: Bind address to socket
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(port); // Use the parsed port number

    if (bind(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0) {
        perror("Error: ");
        return 1;
    }
    if (listen(sockfd, 1) < 0) {
        perror("Error: connection refused");
        return 1;
    }
    printf("Server started on port %d.\n", port);
    
    if ((new_socket = accept(sockfd, (struct sockaddr *)&servaddr, (socklen_t*)&addrlen)) < 0) {
        perror("accept failed");
        exit(EXIT_FAILURE);
    }
    // Call to the server processing function (make sure this function is defined in network.h and implemented in network.c)
    serverProcess(new_socket);
    close(new_socket);
    close(sockfd);
    return 0;
}
