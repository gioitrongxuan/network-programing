#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include <string.h>
#include"user.h"

void printMenu(){
    printf("USER MANAGEMENT PROGRAM\n");
    printf("-----------------------------------\n");
    printf("1. Register\n");
    printf("2. Sign in\n");
    printf("3. Change password\n");
    printf("4. Update account info\n");
    printf("5. Reset password\n");
    printf("6. View login history\n");
    printf("7.Homepage with domain name\n");
    printf("8.Homepage with IP address\n");
    printf("9. Log out\n");
    printf("Your choice (1-9, other to quit):\n");
}
int main(){
    readUsers();
    int choice;
    while (1)
    {
        printMenu();
        choice = 0;
        scanf("%d", &choice);
    switch(choice){
        case 1:
            printf("Register\n");
            registerUser();
            break;
        case 2:
            printf("Sign in\n");
            signIn();
            break;
        case 3:
            printf("Change password\n");
            changePassword();
            break;
        case 4:
            printf("Update account info\n");
            updateAccountInfo();
            break;
        case 5:
            printf("Reset password\n");
            resetPassword();
            break;
        case 6:
            printf("View login history\n");
            viewLoginHistory();
            break;
        case 7:
            printf("Homepage with domain name\n");
            viewHomepageWithDomain();
            break;
        case 8:
            printf("Homepage with IP address\n");
            viewHomepageWithIP();
            break;
        case 9:
            printf("Sign out\n");
            signOut();
            break;
        default:
            printf("Goodbye\n");
            return 0;
            break;
    }
    }
    return 0;
}