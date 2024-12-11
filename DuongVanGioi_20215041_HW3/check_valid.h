#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include <ctype.h>
#include <arpa/inet.h> // For inet_pton

int is_valid_username(const char *username)
{
    int length = strlen(username);
    if (length < 3 || length > 20)
    {
        return 0;
    }
    for (int i = 0; i < length; i++)
    {
        if (isspace(username[i]) || (!isalnum(username[i]) && username[i] != '_' && username[i] != '.'))
        {
            return 0;
        }
    }
    return 1;
}

int is_valid_email(const char *email)
{ 
    int length = strlen(email);
    int at = 0;
    for (int i = 0; i < length; i++)
    {
        if (isspace(email[i]) || (!isalnum(email[i]) && email[i] != '@' && email[i] != '.'))
        {
            return 0;
        }
        if (email[i] == '@')
        {
            at++;
        }
    }
    if (at != 1)
    {
        return 0;
    }
    return 1;
}

int is_valid_phone(const char *phone)
{
    int length = strlen(phone);
    // if (length < 10 || length > 15)
    // {
    //     return 0;
    // }

    for (int i = 0; i < length; i++)
    {
        if (!isdigit(phone[i]))
        {
            return 0;
        }
    }
    return 1;
}

int is_valid_password(const char *password)
{
    int length = strlen(password);
     if (length < 1 )
    {
        return 0;
    }
    for (int i = 0; i < length; i++)
    {
        if (isspace(password[i]) || (!isalnum(password[i]) && password[i] != '_' && password[i] != '.'))
        {
            return 0;
        }
    }
    return 1;
}

int is_valid_ipv4(const char *ip) {
    int num = 0;
    int dots = 0;
    const char *ptr = ip;

    while (*ptr) {
        // Kiểm tra số và xây dựng từng phần
        if (isdigit(*ptr)) {
            num = num * 10 + (*ptr - '0'); // Xây dựng số
            if (num > 255) {
                return 0; // Nếu số lớn hơn 255
            }
        } else if (*ptr == '.') {
            dots++;
            if (num == 0) {
                return 0; // Nếu không có số trước dấu chấm
            }
            num = 0; // Đặt lại số cho phần tiếp theo
        } else {
            return 0; // Ký tự không hợp lệ
        }
        ptr++;
    }

    // Đảm bảo có đúng 3 dấu chấm (4 phần)
    return dots == 3 && num > 0;
}
int is_valid_domain(const char *domain) {
    int length = strlen(domain);
    
    // Kiểm tra độ dài tối thiểu và tối đa của domain
    if (length < 1 || length > 253) {
        return 0;
    }

    // Kiểm tra phần đầu và phần cuối của domain không có dấu chấm
    if (domain[0] == '.' || domain[length - 1] == '.') {
        return 0;
    }

    // Kiểm tra từng ký tự trong domain
    const char *allowed = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-.";
    for (int i = 0; i < length; i++) {
        if (!strchr(allowed, domain[i])) {
            return 0;
        }
    }

    // Kiểm tra xem có ít nhất một dấu chấm trong domain
    if (strchr(domain, '.') == NULL) {
        return 0;
    }

    return 1;
}
int is_valid_homepage(const char *homepage)
{
    return is_valid_ipv4(homepage) || is_valid_domain(homepage);
}