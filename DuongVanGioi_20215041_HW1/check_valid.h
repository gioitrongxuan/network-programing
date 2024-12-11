#include<stdio.h>
#include<stdlib.h>
#include <ctype.h>

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