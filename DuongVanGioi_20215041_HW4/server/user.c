#include "user.h"

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

    printf("Homepage: ");
    fgets(homepage, HOMEPAGE_LEN, stdin);
    homepage[strcspn(homepage, "\n")] = 0; // Remove newline character
    if (!is_valid_homepage(homepage))
    {
        printf("Invalid homepage %s \n", homepage);
        return;
    }

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
                    return WRONG_PASSWORD;
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
        if (!is_valid_homepage(homepage))
        {
            printf("Invalid homepage %s\n", homepage);
            printf("Update homepage failed\n");
            return;
        }
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
    if (!is_valid_username(username))
    {
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

// hw3
void viewHomepageWithDomain()
{
    if (currentUser == NULL)
    {
        printf("You must sign in first\n");
        return;
    }
    if (isIPv4(currentUser->homepage))
    {
        printf("Homepage: %s\n", lookup_ip(currentUser->homepage));
    }
    else if (isDomain(currentUser->homepage))
    {
        printf("Homepage: %s\n", currentUser->homepage);
    }
    else
    {

        printf("Co gi do sai sai\n");
    }
}

void viewHomepageWithIP()
{
    if (currentUser == NULL)
    {
        printf("You must sign in first\n");
        return;
    }

    if (isIPv4(currentUser->homepage))
    {
        printf("Homepage: %s\n", currentUser->homepage);
    }
    else if (isDomain(currentUser->homepage))
    {
        printf("Homepage: %s\n", lookup_domain(currentUser->homepage));
    }
    else
    {
        printf("Co gi do sai sai\n");
    }
}




