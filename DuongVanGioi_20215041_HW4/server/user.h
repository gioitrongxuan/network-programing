#ifndef USER_H
#define USER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "config.h"
#include "net_lookup.h"
#include "check_valid.h"
// Định nghĩa struct User
typedef struct user {
    char username[USERNAME_LEN];
    char password[PASSWORD_LEN];
    int status;
    char homepage[EMAIL_LEN];
    int wrongPasswordCount;
    struct user *next;
} User;
extern User *users;
extern User *currentUser;
// Khai báo các hàm liên quan đến User
User *createUser(char username[], char password[], char homepage[]);
char *formatUserInfo(User *user);
void readUsers();
void printUsers();
void registerUser();
int saveUsers();
int userAuth(char *username, char *password);
void changePassword();
void updatePassword(char *newPassword);
void updateAccountInfo();
void resetPassword();
void viewLoginHistory();
void serverProcess(int sockfd);
void signOut();

#endif
