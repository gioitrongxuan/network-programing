#ifndef ACCOUNT_H
#define ACCOUNT_H

#define MAX_CHARS 1024
#define MAX_ATTEMPTS 3
#define USERNAME_REQUIRED 1
#define PASSWORD_REQUIRED 2
#define VALID_CREDENTIALS 3
#define WRONG_PASSWORD 4
#define ACCOUNT_BLOCKED 5
#define ACCOUNT_ALREADY_SIGNED_IN 6

struct account
{
    char *username;
    char *password;
    int is_active; // 1 for active, 0 for blocked
    int attempts;
    int is_signed_in;
    struct account *next;
};

typedef struct account *Account;

Account new_account(char *username, char *password, int attempts, int is_active, int is_signed_in);

Account add_account(Account account_list, char *username, char *password, int attempts, int is_active, int is_signed_in);

Account read_account(const char *filename);

Account find_account(Account head, const char *username);

int process_login(Account account_list, char *username, char *password);

void save_to_file(Account account_list, const char *filename);

#endif // ACCOUNT_H
