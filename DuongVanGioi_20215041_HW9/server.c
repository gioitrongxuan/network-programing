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

    struct pollfd fds[FD_SETSIZE];
    int nfds = 1; // number of fds
    fds[0].fd = listenfd;
    fds[0].events = POLLIN;

    if (pthread_mutex_init(&lock, NULL) != 0)
    {
        printf("\n Mutex init has failed\n");
        return 1;
    }

    char rcvBuff[MAX_CHARS], sendBuff[MAX_CHARS];
    char *username[FD_SETSIZE], *password[FD_SETSIZE];
    int clientStatus[FD_SETSIZE];
    for (int i = 1; i < FD_SETSIZE; i++)
    {
        username[i] = NULL;
        password[i] = NULL;
        clientStatus[i] = -1;
    }

    // Accept connection and communicate with clients
    while (true)
    {
        int poll_count = poll(fds, nfds, -1);
        if (poll_count == -1)
        {
            perror("Error: ");
            break;
        }
        for (int i = 0; i < nfds; ++i)
        {
            if (fds[i].revents & POLLIN)
            {
                if (fds[i].fd == listenfd)
                {
                    struct sockaddr_in clientAddr;
                    socklen_t clientAddrLen = sizeof(clientAddr);
                    int connfd = accept(listenfd, (struct sockaddr *)&clientAddr, &clientAddrLen);
                    if (connfd == -1)
                    {
                        perror("Error: ");
                        continue;
                    }
                    printf("New accepted connfd: %d\n", connfd);
                    if (nfds < FD_SETSIZE)
                    {
                        fds[nfds].fd = connfd;
                        fds[nfds].events = POLLIN;
                        clientStatus[nfds] = USERNAME_REQUIRED;
                        username[nfds] = (char *)malloc(sizeof(char) * MAX_CHARS);
                        password[nfds] = (char *)malloc(sizeof(char) * MAX_CHARS);
                        nfds++;
                    }
                    else
                    {
                        printf("Max number of clients reached.\n");
                        close(connfd);
                    }
                }
                else
                {
                    memset(rcvBuff, '\0', MAX_CHARS);
                    memset(sendBuff, '\0', MAX_CHARS);
                    int bytes_received = recv(fds[i].fd, rcvBuff, MAX_CHARS, 0);
                    if (bytes_received <= 0)
                    {
                        printf("Client %d has closed connection.\n", fds[i].fd);
                        close(fds[i].fd);
                        fds[i].fd = -1;
                        free(username[i]);
                        free(password[i]);
                        username[i] = NULL;
                        password[i] = NULL;
                        clientStatus[i] = -1;
                    }
                    else
                    {
                        rcvBuff[bytes_received - 1] = '\0';
                        printf("Received from fd %d: %s\n", fds[i].fd, rcvBuff);
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
                                    // Update the new password in the account list
                                    Account account = find_account(account_list, username[i]);
                                    if (account != NULL)
                                    {
                                        strcpy(account->password, rcvBuff); // Save the new password
                                                                            // Send encoded `letters` and `digits` to client
                                        for (int j = 0; j < FD_SETSIZE; ++j)
                                        {
                                            if (fds[j].fd > 0 && clientStatus[j] == VALID_CREDENTIALS && strcmp(username[j], username[i]) == 0 && i != j)
                                            {
                                                int bytes_sent = send(fds[j].fd, sendBuff, strlen(sendBuff), 0);
                                                if (bytes_sent <= 0)
                                                {
                                                    printf("Failed to send message to client %d\n", fds[j].fd);
                                                }
                                            }
                                        }
                                    }

                                    // Save the account list to file
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
                        int bytes_sent = send(fds[i].fd, sendBuff, strlen(sendBuff), 0);
                        if (bytes_sent <= 0)
                        {
                            printf("Connection closed.\n");
                            close(fds[i].fd);
                            fds[i].fd = -1;
                            free(username[i]);
                            free(password[i]);
                            username[i] = NULL;
                            password[i] = NULL;
                            clientStatus[i] = -1;
                        }
                    }
                }
            }
        }
        // Remove closed connections
        for (int i = 0; i < nfds;)
        {
            if (fds[i].fd == -1)
            {
                for (int j = i; j < nfds - 1; ++j)
                {
                    fds[j] = fds[j + 1];
                    username[j] = username[j + 1];
                    password[j] = password[j + 1];
                    clientStatus[j] = clientStatus[j + 1];
                }
                nfds--;
            }
            else
            {
                i++;
            }
        }
    }

    pthread_mutex_destroy(&lock);
    close(listenfd);
    printf("Server closed.\n");
    return 0;
}
