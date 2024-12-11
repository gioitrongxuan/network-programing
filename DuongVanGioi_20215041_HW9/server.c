#include "account.h"
#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <ctype.h>
#include <poll.h>

const int BACKLOG = 20;
const char *FILENAME = "account.txt";

pthread_mutex_t lock;


Account account_list = NULL;

int is_valid_password(const char *password)
{
    for (int i = 0; password[i] != '\0'; i++)
    {
        if (!isalnum(password[i]))
        {
            return 0;
        }
    }
    return 1;
}

void split_password(const char *password, char *letters, char *digits)
{
    int letter_index = 0, digit_index = 0;
    for (int i = 0; password[i] != '\0'; i++)
    {
        if (isalpha(password[i]))
        {
            letters[letter_index++] = password[i];
        }
        else if (isdigit(password[i]))
        {
            digits[digit_index++] = password[i];
        }
    }
    letters[letter_index] = '\0';
    digits[digit_index] = '\0';
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Usage: ./server <port_number>\n");
        return 0;
    }
    char *endstr;
    errno = 0;
    uint16_t PORT = strtoul(argv[1], &endstr, 10);

    if (errno != 0 || *endstr != '\0')
    {
        printf("Error: %s\n", errno == EINVAL ? "invalid base" : "invalid input");
        return 0;
    }

    account_list = read_account(FILENAME);

    // Construct socket
    int listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if (listenfd == -1)
    {
        perror("Error: ");
        return 0;
    }
    // Bind address to socket
    struct sockaddr_in server;
    bzero(&server, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    server.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(listenfd, (struct sockaddr *)&server, sizeof(server)) == -1)
    {
        perror("Error: ");
        return 0;
    }
    // Listen to requests
    if (listen(listenfd, BACKLOG) == -1)
    {
        perror("Error: ");
        return 0;
    }

    int client[FD_SETSIZE], clientStatus[FD_SETSIZE], connfd;
    fd_set checkfds, readfds;
    struct sockaddr_in clientAddr;
    int nEvents;
    socklen_t clientAddrLen;
    char rcvBuff[MAX_CHARS], sendBuff[MAX_CHARS];
    char *username[FD_SETSIZE], *password[FD_SETSIZE];

    for (int i = 0; i < FD_SETSIZE; ++i)
    {
        client[i] = -1;
        clientStatus[i] = -1;
        username[i] = NULL;
        password[i] = NULL;
    }

    FD_ZERO(&checkfds);
    FD_SET(listenfd, &checkfds);
    int maxfd = listenfd;

    if (pthread_mutex_init(&lock, NULL) != 0)
    {
        printf("\n Mutex init has failed\n");
        return 1;
    }

    // Accept connection and communicate with clients
    while (true)
    {
        int i;
        readfds = checkfds;
        nEvents = select(maxfd + 1, &readfds, NULL, NULL, NULL);
        if (nEvents == -1)
        {
            perror("Error: ");
            break;
        }
        else if (nEvents == 0)
        {
            printf("Timeout.\n");
            break;
        }
        if (FD_ISSET(listenfd, &readfds))
        {
            clientAddrLen = sizeof(clientAddr);
            connfd = accept(listenfd, (struct sockaddr *)(&clientAddr), &clientAddrLen);
            printf("New accepted connfd: %d\n", connfd);
            for (i = 0; i < FD_SETSIZE; ++i)
            {
                if (client[i] <= 0)
                {
                    client[i] = connfd;
                    clientStatus[i] = USERNAME_REQUIRED;
                    username[i] = (char *)malloc(sizeof(char) * MAX_CHARS);
                    password[i] = (char *)malloc(sizeof(char) * MAX_CHARS);
                    FD_SET(client[i], &checkfds);
                    if (connfd > maxfd)
                        maxfd = connfd;
                    break;
                }
                if (--nEvents <= 0)
                    continue;
            }
            if (i == FD_SETSIZE)
            {
                printf("Max number of clients reached.");
            }
        }

        for (i = 0; i < FD_SETSIZE; ++i)
        {
            if (client[i] <= 0)
                continue;
            if (FD_ISSET(client[i], &readfds))
            {
                int hasClosed = 0;
                memset(rcvBuff, '\0', MAX_CHARS);
                memset(sendBuff, '\0', MAX_CHARS);
                int bytes_received = recv(client[i], rcvBuff, MAX_CHARS, 0);
                if (bytes_received <= 0)
                {
                    printf("Connection closed.\n");
                    FD_CLR(client[i], &checkfds);
                    close(client[i]);
                    client[i] = -1;
                    clientStatus[i] = -1;
                    free(username[i]);
                    free(password[i]);
                    username[i] = NULL;
                    password[i] = NULL;
                    hasClosed = 1;
                }
                else
                {
                    rcvBuff[bytes_received - 1] = '\0';
                    printf("Received from fd %d: %s\n", client[i], rcvBuff);
                    switch (clientStatus[i])
                    {
                    case USERNAME_REQUIRED:
                    {
                        if (strcmp(rcvBuff, "") == 0)
                        {
                            strcpy(sendBuff, "goodbye");
                            memset(rcvBuff, '\0', MAX_CHARS);
                        }
                        else
                        {
                            strcpy(username[i], rcvBuff);
                            strcpy(sendBuff, "insert password: ");
                            clientStatus[i] = PASSWORD_REQUIRED;
                        }
                        break;
                    }
                    case PASSWORD_REQUIRED:
                    {
                        strcpy(password[i], rcvBuff);
                        clientStatus[i] = process_login(account_list, username[i], password[i]);
                        switch (clientStatus[i])
                        {
                        case USERNAME_REQUIRED:
                        {
                            strcpy(sendBuff, "username does not exist");
                            memset(username[i], '\0', MAX_CHARS);
                            memset(password[i], '\0', MAX_CHARS);
                            break;
                        }
                        case ACCOUNT_ALREADY_SIGNED_IN:
                        {
                            clientStatus[i] = USERNAME_REQUIRED;
                            strcpy(sendBuff, "account is already signed in");
                            memset(username[i], '\0', MAX_CHARS);
                            memset(password[i], '\0', MAX_CHARS);
                            break;
                        }
                        case VALID_CREDENTIALS:
                        {
                            strcpy(sendBuff, "OK");
                            break;
                        }
                        case WRONG_PASSWORD:
                        {
                            clientStatus[i] = USERNAME_REQUIRED;
                            strcpy(sendBuff, "Not OK");
                            memset(username[i], '\0', MAX_CHARS);
                            memset(password[i], '\0', MAX_CHARS);
                            break;
                        }
                        case ACCOUNT_BLOCKED:
                        {
                            clientStatus[i] = USERNAME_REQUIRED;
                            strcpy(sendBuff, "Account is blocked");
                            memset(username[i], '\0', MAX_CHARS);
                            memset(password[i], '\0', MAX_CHARS);
                            break;
                        }
                        }
                        break;
                    }
                    case VALID_CREDENTIALS:
                    {
                        if (strcmp(rcvBuff, "bye") == 0)
                        {
                            clientStatus[i] = USERNAME_REQUIRED;
                            strcpy(sendBuff, "Goodbye");
                            memset(username[i], '\0', MAX_CHARS);
                            memset(password[i], '\0', MAX_CHARS);
                        }
                        else
                        {
                            // Process change password request
                            if (is_valid_password(rcvBuff))
                            {
                                char letters[50], digits[50];
                                split_password(rcvBuff, letters, digits);

                               
                                sprintf(sendBuff, "Password changed successfully\nLetters: %s\nDigits: %s\n", letters, digits);
                                // Cập nhật mật khẩu mới vào danh sách tài khoản
                                Account account = find_account(account_list, username[i]);
                                if (account != NULL)
                                {
                                    strcpy(account->password, rcvBuff); // Lưu mật khẩu mới
                                     // Gửi mã hóa `letters` và `digits` cho client
                                    for (int j = 0; j < FD_SETSIZE; ++j)
                                        {
                                            if (client[j] > 0 && clientStatus[j] == VALID_CREDENTIALS && strcmp(username[j],username[i]) == 0 && i != j)
                                            {
                                                int bytes_sent = send(client[j],sendBuff , strlen(sendBuff), 0);
                                                if (bytes_sent <= 0)
                                                {
                                                    printf("Failed to send message to client %d\n", client[j]);
                                                }
                                            }
                                        }
                                }

                                // Lưu danh sách tài khoản vào file
                                pthread_mutex_lock(&lock);
                                save_to_file(account_list, FILENAME);
                                pthread_mutex_unlock(&lock);
                            }
                            else
                            {
                                strcpy(sendBuff, "Error: Invalid password format. Only letters and digits are allowed.");
                            }
                        }
                        break;
                    }
                    }
                    int bytes_sent = send(client[i], sendBuff, strlen(sendBuff), 0);
                    sendBuff[bytes_sent] = '\0';
                    if (bytes_sent <= 0)
                    {
                        printf("Connection closed.\n");
                        FD_CLR(client[i], &checkfds);
                        close(client[i]);
                        client[i] = -1;
                        clientStatus[i] = -1;
                        free(username[i]);
                        free(password[i]);
                        username[i] = NULL;
                        password[i] = NULL;
                        hasClosed = 1;
                    }
                }
                if (hasClosed)
                {
                    pthread_mutex_lock(&lock);
                    save_to_file(account_list, FILENAME);
                    pthread_mutex_unlock(&lock);
                }
                if (--nEvents <= 0)
                    continue;
            }
        }
    }

    pthread_mutex_destroy(&lock);
    close(listenfd);
    printf("Server closed.\n");
    return 0;
}
