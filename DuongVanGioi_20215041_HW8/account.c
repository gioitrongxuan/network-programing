#include "account.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "user.h"

// Tạo tài khoản mới
Account new_account(char *username, char *password, int attempts, int is_active, int is_signed_in)
{
    Account acc = (Account)malloc(sizeof(struct account));
    acc->username = (char *)malloc(sizeof(char) * MAX_CHARS);
    acc->password = (char *)malloc(sizeof(char) * MAX_CHARS);
    strcpy(acc->username, username);
    strcpy(acc->password, password);
    acc->is_active = is_active;
    acc->attempts = attempts;
    acc->is_signed_in = is_signed_in;
    acc->next = NULL;
    return acc;
}

// Thêm tài khoản mới vào danh sách tài khoản
Account add_account(Account account_list, char *username, char *password, int attempts, int is_active, int is_signed_in)
{
    Account new_acc = new_account(username, password, attempts, is_active, is_signed_in);
    if (account_list == NULL)
    {
        account_list = new_acc;
    }
    else
    {
        Account tmp = account_list;
        while (tmp->next != NULL)
        {
            tmp = tmp->next;
        }
        tmp->next = new_acc;
    }
    return account_list;
}

// Đọc thông tin tài khoản từ file
Account read_account(const char *filename)
{
    FILE *f = fopen(filename, "r");
    if (f == NULL)
    {
        printf("Cannot open file %s to read account information!\n", filename);
        return NULL;
    }
    Account account_list = NULL;
    char *username = (char *)malloc(sizeof(char) * MAX_CHARS);
    char *password = (char *)malloc(sizeof(char) * MAX_CHARS);
    int is_active;
    while (fscanf(f, "%s %s %d", username, password, &is_active) != EOF)
    {
        account_list = add_account(account_list, username, password, 0, is_active, 0);
    }
    fclose(f);
    return account_list;
}

Account find_account(Account head, const char *username)
{
    Account current = head;
    while (current != NULL)
    {
        if (strcmp(current->username, username) == 0)
        {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

// Xử lý đăng nhập
int process_login(Account account_list, char *username, char *password)
{
    Account tmp = account_list;
    while (tmp != NULL)
    {
        if (strcmp(tmp->username, username) == 0)
        {
            if (strcmp(tmp->password, password) == 0)
            {
                // if (tmp->is_signed_in)
                // {
                //     return ACCOUNT_ALREADY_SIGNED_IN; // Tài khoản đã đăng nhập
                // }
                // else 
                if (tmp->is_active == 1)
                {
                    tmp->is_signed_in = 1;
                    tmp->attempts = 0;
                    return VALID_CREDENTIALS; // Mật khẩu hợp lệ
                }
                else
                {
                    return ACCOUNT_BLOCKED; // Tài khoản bị khóa
                }
            }
            else
            {
                tmp->attempts++;
                if (tmp->attempts >= MAX_ATTEMPTS)
                {
                    tmp->is_active = 0;
                    return ACCOUNT_BLOCKED; // Tài khoản bị khóa sau 3 lần nhập sai
                }
                else
                {
                    return WRONG_PASSWORD; // Mật khẩu sai
                }
            }
        }
        tmp = tmp->next;
    }
    return USERNAME_REQUIRED; // Tên đăng nhập không tồn tại
}

// Lưu thông tin tài khoản vào file
void save_to_file(Account account_list, const char *filename)
{
    FILE *f = fopen(filename, "w+");
    if (f == NULL)
    {
        printf("Cannot open file %s to save account information!\n", filename);
        return;
    }
    Account tmp = account_list;
    while (tmp->next != NULL)
    {
        fprintf(f, "%s %s %d\n", tmp->username, tmp->password, tmp->is_active);
        tmp = tmp->next;
    }
    fprintf(f, "%s %s %d", tmp->username, tmp->password, tmp->is_active);
    fclose(f);
}




User *users = NULL;
User *currentUser = NULL;

User *createUser(char username[], char password[], char homepage[])
{
    User *newUser = (User *)malloc(sizeof(User));
    strcpy(newUser->username, username);
    strcpy(newUser->password, password);
    strcpy(newUser->homepage, homepage);
    newUser->status = ACTIVE;
    newUser->next = NULL;
    newUser->wrongPasswordCount = 0;
    return newUser;
}

char *formatUserInfo(User *user)
{
    static char info[200];
    sprintf(info, FORMAT_USER_INFOR, user->username, user->password, user->status, user->homepage);
    return info;
}

void readUsers()
{
    char username[USERNAME_LEN];
    char password[PASSWORD_LEN];
    int status;
    char homepage[EMAIL_LEN];
    FILE *f = fopen(ACCOUNT_FILE, "r");
    if (f == NULL)
    {
        printf("Cannot open file users information\n");
        return;
    }
    User *tmp = NULL;
    while (fscanf(f, SCAN_FORMAT_USER_INFOR, username, password, &status, homepage) != EOF)
    {
        if (users != NULL)
        {
            tmp->next = createUser(username, password, homepage);
            tmp = tmp->next;
        }
        else
        {
            users = createUser(username, password, homepage);
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
        printf(FORMAT_USER_INFOR, tmp->username, tmp->password, tmp->status, tmp->homepage);
        tmp = tmp->next;
    }
}

void registerUser()
{
    char username[USERNAME_LEN];
    char password[PASSWORD_LEN];
    char homepage[HOMEPAGE_LEN];
    fflush(stdin);
    printf("Username: ");
    fgets(username, USERNAME_LEN, stdin);
    username[strcspn(username, "\n")] = 0;

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
 
    printf("Homepage: ");
    fgets(homepage, HOMEPAGE_LEN, stdin);
    homepage[strcspn(homepage, "\n")] = 0; // Remove newline character
   

    User *newUser = createUser(username, password, homepage);
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

int userAuth(char *username, char *password)
{

    User *tmp = users;
    while (tmp != NULL)
    {
        if (strcmp(tmp->username, username) == 0)
        {
            if (tmp->status == BLOCKED)
            {
                printf("Your account is blocked.\n");
                return BLOCKED;
            }
            while (tmp->wrongPasswordCount < 3)
            {

                if (strcmp(tmp->password, password) == 0)
                {
                    FILE *f = fopen(HISTORY_FILE, "a");
                    if (f == NULL)
                    {
                        printf("Cannot open login history file.\n");
                    }
                    time_t now;
                    time(&now);
                    struct tm *local = localtime(&now);
                    fprintf(f, FORMAT_HISTORY_INFOR, tmp->username, local->tm_mday, local->tm_mon + 1, local->tm_year + 1900, local->tm_hour, local->tm_min, local->tm_sec);
                    fclose(f);
                    printf("Welcome\n");
                    tmp->wrongPasswordCount = 0;
                    currentUser = tmp;
                    return ACTIVE;
                }
                else
                {
                    tmp->wrongPasswordCount++;
                    return WRONG_PASSWORD_2;
                }
            }
            if (tmp->wrongPasswordCount == 3)
            {
                tmp->status = BLOCKED;
                saveUsers();
                return TOO_MANY_WRONG_PASSWORD;
            }
        }
        tmp = tmp->next;
    }
   return NOT_FOUND;
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
    // check old password
    printf("Old password: ");
    fgets(password, PASSWORD_LEN, stdin);
    password[strcspn(password, "\n")] = 0;
    if (strcmp(currentUser->password, password) != 0)
    {
        printf("Incorrect password\n");
        return;
    }
    // check new password
    printf("New password: ");
    fgets(password, PASSWORD_LEN, stdin);
    password[strcspn(password, "\n")] = 0;
    

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
void updatePassword(char *newPassword)
{
    if (currentUser == NULL)
        return;
    strcpy(currentUser->password, newPassword);
    saveUsers();
}

void updateAccountInfo()
{
    if (currentUser == NULL)
    {
        printf("You must sign in first\n");
        return;
    }
    printf("Update account info\n");
    printf("1. New homepage\n");
    printf("2. New ...\n");
    printf("Your choice: ");
    char homepage[HOMEPAGE_LEN];
    int choice = 0;
    scanf("%d", &choice);
    getchar(); // Consume newline character left by scanf
    switch (choice)
    {
    case 1:
        printf("New homepage: ");
        fgets(homepage, HOMEPAGE_LEN, stdin);
        homepage[strcspn(homepage, "\n")] = 0; // Remove newline character
       
        strcpy(currentUser->homepage, homepage);
        if (saveUsers())
        {
            printf("Update homepage successfully\n");
        }
        else
        {
            printf("Update homepage failed\n");
        }
        break;
    case 2:
        printf("HIHI\n");
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

