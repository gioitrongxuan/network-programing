#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#define MAX_USERS 100
#define USERNAME_LEN 50
#define PASSWORD_LEN 50
#define HOMEPAGE_LEN 100
#define EMAIL_LEN 100
#define PHONE_LEN 15
#define OTP_LEN 7
#define ACTIVE 1
#define BLOCKED 0
#define OTP "123456"
#define HISTORY_FILE "history.txt"
#define ACCOUNT_FILE "account.txt"
#define FORMAT_USER_INFOR "%s %s %d %s\n"
#define SCAN_FORMAT_USER_INFOR "%s %s %d %s"
#define FORMAT_HISTORY_INFOR "%s | %02d-%02d-%04d | %02d:%02d:%02d\n"
#define NOT_FOUND -1
#define TOO_MANY_WRONG_PASSWORD 3
#define WRONG_PASSWORD 2