#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "auth_form.h"
#define MAX_BUFFER 1024

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        printf("Usage: ./client <IPAddress> <PortNumber>\n");
        return 1;
    }

    char *ipAddress = argv[1];
    int port = atoi(argv[2]);
    int udp_sockfd;
    struct sockaddr_in serverAddr;

    // Tạo socket UDP
    udp_sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (udp_sockfd < 0)
    {
        perror("UDP socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    serverAddr.sin_addr.s_addr = inet_addr(ipAddress);

    printf("Client is running. Type 'exit' to quit.\n");

    while (1)
    {
        signIn(udp_sockfd, &serverAddr); // Gọi hàm signIn với sockfd và địa chỉ server
        // Nhận phản hồi từ server
        char buffer[MAX_BUFFER];
        socklen_t addrLen = sizeof(serverAddr);
        int n = recvfrom(udp_sockfd, buffer, MAX_BUFFER, 0, (struct sockaddr *)&serverAddr, &addrLen);
        if (n < 0)
        {
            perror("Failed to receive data from server");
            close(udp_sockfd);
            return 1;
        }
        buffer[n] = '\0'; // Kết thúc chuỗi nhận được

        printf("Server response: %s\n", buffer);
        if (strcmp(buffer, "OK") == 0)
        {
            while (1)
            {
                char input[MAX_BUFFER];
                fflush(stdin);
                printf("Enter new password, 'homepage' to view homepage, 'bye' to logout: ");
                fgets(input, MAX_BUFFER, stdin);
                input[strcspn(input, "\n")] = 0;
                sendto(udp_sockfd, input, strlen(input), 0, (const struct sockaddr *)&serverAddr, sizeof(serverAddr));

                int m = recvfrom(udp_sockfd, buffer, MAX_BUFFER, 0, (struct sockaddr *)&serverAddr, &addrLen);
               
                if (m < 0)
                {
                    perror("Failed to receive data from server");
                    close(udp_sockfd);
                    return 1;
                }
                buffer[m] = '\0'; 
                printf("Server response: %s\n", buffer);
               
            }
        }
        if (strcmp(buffer, "exit") == 0)
        {
            printf("Server requested to close the connection.\n");
            return 0;
        }
    }

    close(udp_sockfd);
    return 0;
}
