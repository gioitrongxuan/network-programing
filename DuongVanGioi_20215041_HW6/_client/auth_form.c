#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "config.h"

void signIn(int sockfd, struct sockaddr_in *serverAddr) {
    char username[USERNAME_LEN];
    char password[PASSWORD_LEN];
    char buffer[USERNAME_LEN + PASSWORD_LEN + 2]; // Để chứa username và password

    fflush(stdin);
    printf("Username: ");
    fgets(username, USERNAME_LEN, stdin);
    username[strcspn(username, "\n")] = 0; // Xóa ký tự newline

    printf("Password: ");
    fgets(password, PASSWORD_LEN, stdin);
    password[strcspn(password, "\n")] = 0; // Xóa ký tự newline

    // Gửi username và password tới server
    sprintf(buffer, "%s %s", username, password);
    sendto(sockfd, buffer, strlen(buffer), 0, (const struct sockaddr *)serverAddr, sizeof(*serverAddr));
    printf("Sent: %s\n", buffer);
}
