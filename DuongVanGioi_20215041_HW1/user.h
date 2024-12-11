#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "check_valid.h"

#define MAX_USERS 100
#define USERNAME_LEN 50
#define PASSWORD_LEN 50
#define EMAIL_LEN 100
#define PHONE_LEN 15
#define OTP_LEN 7
#define ACTIVE 1
#define BLOCKED 0
#define OTP "123456"
#define HISTORY_FILE "history.txt"
#define ACCOUNT_FILE "account.txt"
#define FORMAT_USER_INFOR "%s %s %s %s %d\n"
#define FORMAT_HISTORY_INFOR "%s | %02d-%02d-%04d | %02d:%02d:%02d\n"

typedef struct user
{
    char username[USERNAME_LEN];
    char password[PASSWORD_LEN];
    char email[EMAIL_LEN];
    char phone[PHONE_LEN];
    int status;
    int wrongPasswordCount;
    struct user *next;
} User;

User *users = NULL;
User *currentUser = NULL;

User *createUser(char username[], char password[], char email[], char phone[])
{
    User *newUser = (User *)malloc(sizeof(User));
    strcpy(newUser->username, username);
    strcpy(newUser->password, password);
    strcpy(newUser->email, email);
    strcpy(newUser->phone, phone);
    newUser->status = ACTIVE;
    newUser->next = NULL;
    newUser->wrongPasswordCount = 0;
    return newUser;
}

char *formatUserInfo(User *user)
{
    static char info[200];
    sprintf(info, FORMAT_USER_INFOR, user->username, user->password, user->email, user->phone, user->status);
    return info;
}

void readUsers()
{
    char username[USERNAME_LEN];
    char password[PASSWORD_LEN];
    char email[EMAIL_LEN];
    char phone[PHONE_LEN];
    int status;
    FILE *f = fopen(ACCOUNT_FILE, "r");
    if (f == NULL)
    {
        printf("Cannot open file users information\n");
        return;
    }
    User *tmp = NULL;
    while (fscanf(f, "%s %s %s %s %d", username, password, email, phone, &status) != EOF)
    {
        if (users != NULL)
        {
            tmp->next = createUser(username, password, email, phone);
            tmp = tmp->next;
        }
        else
        {
            users = createUser(username, password, email, phone);
            tmp = users;
        }
    }
    fclose(f);
}

void printUsers()
{
    User *tmp = users;
    while (tmp != NULL)
    {
        printf("%s %s %s %s %d\n", tmp->username, tmp->password, tmp->email, tmp->phone, tmp->status);
        tmp = tmp->next;
    }
}

void registerUser()
{
    char username[USERNAME_LEN];
    char password[PASSWORD_LEN];
    char email[EMAIL_LEN];
    char phone[PHONE_LEN];
    fflush(stdin);
    printf("Username: ");
    fgets(username, USERNAME_LEN, stdin);
    username[strcspn(username, "\n")] = 0;

    if (!is_valid_username(username))
    {
        printf("Invalid username\n");
        return;
    }

    User *tmp = users;
    while (tmp != NULL)
    {
        if (strcmp(tmp->username, username) == 0)
        {
            printf("Username already exists\n");
            return;
        }
        tmp = tmp->next;
    }

    printf("Password: ");
    fgets(password, PASSWORD_LEN, stdin);
    password[strcspn(password, "\n")] = 0; // Remove newline character
    if (!is_valid_password(password))
    {
        printf("Invalid password \"%s\"\n", password);
        return;
    }


    printf("Email: ");
    fgets(email, EMAIL_LEN, stdin);
    email[strcspn(email, "\n")] = 0; // Remove newline character
    if (!is_valid_email(email))
    {
        printf("Invalid email\n");
        return;
    }

    printf("Phone: ");
    fgets(phone, PHONE_LEN, stdin);
    phone[strcspn(phone, "\n")] = 0; // Remove newline character
    if (!is_valid_phone(phone))
    {
        printf("Invalid phone\n");
        return;
    }

    User *newUser = createUser(username, password, email, phone);
    printf("%s\n", formatUserInfo(newUser));
    FILE *f = fopen(ACCOUNT_FILE, "a");
    if (f == NULL)
    {
        printf("Cannot open account file for writing.\n");
        return;
    }
    fprintf(f, "%s", formatUserInfo(newUser));
    fclose(f);

    if (users == NULL)
    {
        users = newUser;
    }
    else
    {
        tmp = users;
        while (tmp->next != NULL)
        {
            tmp = tmp->next;
        }
        tmp->next = newUser;
    }
    printf("Register successfully\n");
}

int saveUsers()
{
    FILE *f = fopen("new_account.txt", "w");
    if (f == NULL)
    {
        printf("Cannot open file for writing.\n");
        return 0;
    }
    User *tmp = users;
    while (tmp != NULL)
    {
        if (fprintf(f, "%s", formatUserInfo(tmp)) < 0)
        {
            printf("Error writing to file.\n");
            fclose(f);
            return 0;
        }
        tmp = tmp->next;
    }
    fclose(f);
    if (remove(ACCOUNT_FILE) != 0)
    {
        printf("Error removing old account file.\n");
        return 0;
    }
    if (rename("new_account.txt", ACCOUNT_FILE) != 0)
    {
        printf("Error renaming new account file.\n");
        return 0;
    }
    return 1;
}

void signIn()
{
    if (currentUser != NULL)
    {
        printf("You are already signed in\n");
        return;
    }
    char username[USERNAME_LEN];
    char password[PASSWORD_LEN];
    fflush(stdin);
    printf("Username: ");
    fgets(username, USERNAME_LEN, stdin);
    username[strcspn(username, "\n")] = 0; // Remove newline character

    User *tmp = users;
    while (tmp != NULL)
    {
        if (strcmp(tmp->username, username) == 0)
        {
            if (tmp->status == BLOCKED)
            {
                printf("Your account is blocked.\n");
                return;
            }
            while (tmp->wrongPasswordCount < 3)
            {
                printf("Password: ");
                fgets(password, PASSWORD_LEN, stdin);
                password[strcspn(password, "\n")] = 0; // Remove newline character

                if (strcmp(tmp->password, password) == 0)
                {
                    FILE *f = fopen(HISTORY_FILE, "a");
                    if (f == NULL)
                    {
                        printf("Cannot open login history file.\n");
                        return;
                    }
                    time_t now;
                    time(&now);
                    struct tm *local = localtime(&now);
                    fprintf(f, FORMAT_HISTORY_INFOR, tmp->username, local->tm_mday, local->tm_mon + 1, local->tm_year + 1900, local->tm_hour, local->tm_min, local->tm_sec);
                    fclose(f);
                    printf("Welcome\n");
                    tmp->wrongPasswordCount = 0;
                    currentUser = tmp;
                    return;
                }
                else
                {
                    printf("Incorrect password. Try again.\n");
                    tmp->wrongPasswordCount++;
                }
            }
            if (tmp->wrongPasswordCount == 3)
            {
                printf("Too many incorrect attempts. Your account is now blocked.\n");
                tmp->status = BLOCKED;
                saveUsers();
            }
            return;
        }
        tmp = tmp->next;
    }
    printf("Username does not exist.\n");
}

void changePassword()
{
    if (currentUser == NULL)
    {
        printf("You must sign in first\n");
        return;
    }
    fflush(stdin);
    char password[PASSWORD_LEN];
    printf("New password: ");
    fgets(password, PASSWORD_LEN, stdin);
    password[strcspn(password, "\n")] = 0;
    if (!is_valid_password(password))
    {
        printf("Invalid password\n");
        printf("Change password failed\n");
        return;
    }

    strcpy(currentUser->password, password);
    if (saveUsers())
    {
        printf("Change password successfully\n");
    }
    else
    {
        printf("Change password failed\n");
    }
}

void updateAccountInfo()
{
    if (currentUser == NULL)
    {
        printf("You must sign in first\n");
        return;
    }
    printf("Update account info\n");
    printf("1. New email\n");
    printf("2. New phone\n");
    printf("Your choice: ");
    char email[EMAIL_LEN];
    char phone[PHONE_LEN];
    int choice = 0;
    scanf("%d", &choice);
    getchar(); // Consume newline character left by scanf
    switch (choice)
    {
    case 1:
        printf("New email: ");
        fgets(email, EMAIL_LEN, stdin);
        email[strcspn(email, "\n")] = 0; // Remove newline character
        if(!is_valid_email(email)){
            printf("Invalid email\n");
            printf("Update email failed\n");
            return;
        }
        strcpy(currentUser->email, email);
        if (saveUsers())
        {
            printf("Update email successfully\n");
        }
        else
        {
            printf("Update email failed\n");
        }
        break;
    case 2:
        printf("New phone: ");
        fgets(phone, PHONE_LEN, stdin);
        phone[strcspn(phone, "\n")] = 0; // Remove newline character
        strcpy(currentUser->phone, phone);
        if (saveUsers())
        {
            printf("Update phone successfully\n");
        }
        else
        {
            printf("Update phone failed\n");
        }
        break;
    default:
        printf("Invalid choice\n");
        break;
    }
}

void resetPassword()
{
    char username[USERNAME_LEN];
    char otp[OTP_LEN];
    fflush(stdin);
    printf("Username: ");
    fgets(username, USERNAME_LEN, stdin);
    username[strcspn(username, "\n")] = 0; // Remove newline character
    if(!is_valid_username(username)){
        printf("Invalid username\n");
        return;
    }

    printf("OTP: ");
    fgets(otp, OTP_LEN, stdin);
    otp[strcspn(otp, "\n")] = 0; // Remove newline character

    if (strcmp(otp, OTP) != 0)
    {
        printf("Invalid OTP\n");
        return;
    }
    User *tmp = users;
    while (tmp != NULL)
    {
        if (strcmp(tmp->username, username) == 0)
        {
            fflush(stdin);
            printf("New password: ");
            fgets(tmp->password, PASSWORD_LEN, stdin);
            tmp->password[strcspn(tmp->password, "\n")] = 0; // Remove newline character
            if (!is_valid_password(tmp->password))
            {
                printf("Invalid password\n");
                printf("Reset password failed\n");
                return;
            }
            if (saveUsers())
            {
                printf("Reset password successfully\n");
            }
            else
            {
                printf("Reset password failed\n");
            }
            return;
        }
        tmp = tmp->next;
    }
    printf("Username does not exist.\n");
}

void viewLoginHistory()
{
    if (currentUser == NULL)
    {
        printf("You must sign in first\n");
        return;
    }
    FILE *f = fopen(HISTORY_FILE, "r");
    if (f == NULL)
    {
        printf("Cannot open login history file\n");
        return;
    }
    char username[USERNAME_LEN];
    int day, month, year, hour, minute, second;
    while (fscanf(f, FORMAT_HISTORY_INFOR, username, &day, &month, &year, &hour, &minute, &second) != EOF)
    {
        if (strcmp(username, currentUser->username) == 0)
        {
            printf("%s | %02d-%02d-%04d | %02d:%02d:%02d\n", username, day, month, year, hour, minute, second);
        }
    }
    fclose(f);
}

void signOut()
{
    if (currentUser == NULL)
    {
        printf("You must sign in first\n");
        return;
    }
    currentUser = NULL;
    printf("Sign out successfully\n");
}