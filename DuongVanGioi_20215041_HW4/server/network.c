#include "network.h"
#include <arpa/inet.h>

void encryptPassword(char *password, char *lettersOnly, char *digitsOnly)
{
    int l_idx = 0, d_idx = 0;
    for (int i = 0; i < strlen(password); i++)
    {
        if (isalpha(password[i]))
        {
            lettersOnly[l_idx++] = password[i];
        }
        else if (isdigit(password[i]))
        {
            digitsOnly[d_idx++] = password[i];
        }
    }
    lettersOnly[l_idx] = '\0';
    digitsOnly[d_idx] = '\0';
}
void serverProcess(int sockfd)
{
    char buffer[MAX_BUFFER];
    struct sockaddr_in clientAddr;
    socklen_t addrLen = sizeof(clientAddr);
    while (1)
    {
        int n = recvfrom(sockfd, buffer, BUFF_SIZE, 0, (struct sockaddr *)&clientAddr, &addrLen);
        if (n < 0)
        {
            perror("Failed to receive data from client");
            continue;
        }
        buffer[n] = '\0';
        if (currentUser == NULL)
        {
            char username[50], password[50];
            if (sscanf(buffer, "%s %s", username, password) != 2)
            {
                strcpy(buffer, "Invalid input format. Please provide username and password.");
            }
            else
            {
                int result = userAuth(username, password);
                switch (result)
                {
                case ACTIVE:
                    strcpy(buffer, "OK");
                    break;
                case BLOCKED:
                    strcpy(buffer, "Account not ready");
                    break;
                case WRONG_PASSWORD:
                    strcpy(buffer, "Not OK");
                    break;
                case NOT_FOUND:
                    strcpy(buffer, "Account not found.");
                    break;
                case TOO_MANY_WRONG_PASSWORD:
                    strcpy(buffer, "Account is blocked.");
                    break;
                default:
                    strcpy(buffer, "Unknown error.");
                    break;
                }
            }
        }
        else
        {
            if (strcmp(buffer, "bye") == 0)
            {
                signOut();
                strcpy(buffer, "Signed out successfully.");
            }
            else if (strcmp(buffer, "homepage") == 0)
            {
                if (isIPv4(currentUser->homepage))
                {
                    snprintf(buffer, BUFF_SIZE, "Homepage: %s", lookup_ip(currentUser->homepage));
                }
                else if (isDomain(currentUser->homepage))
                {
                    snprintf(buffer, BUFF_SIZE, "Homepage: %s", currentUser->homepage);
                }
                else
                {
                    strcpy(buffer, "Invalid homepage format.");
                }
            }
            else if (is_valid_password(buffer))
            {
                char lettersOnly[BUFF_SIZE] = "";
                char digitsOnly[BUFF_SIZE] = "";
                encryptPassword(buffer, lettersOnly, digitsOnly);
                updatePassword(buffer);

                char message[BUFF_SIZE];
                int length = snprintf(message, BUFF_SIZE, "Password updated: Letters [%s], Digits [%s]", lettersOnly, digitsOnly);

                if (length >= BUFF_SIZE)
                {
                    snprintf(buffer, BUFF_SIZE, "Password updated: Letters [%.*s], Digits [%.*s]",
                             BUFF_SIZE - 40, lettersOnly, BUFF_SIZE - 40, digitsOnly);
                }
                else
                {
                    strcpy(buffer, message);
                }
            }
            else
            {
                strcpy(buffer, "Unknown command.");
            }
        }
        sendto(sockfd, buffer, strlen(buffer), 0, (struct sockaddr *)&clientAddr, addrLen);
    }
}
