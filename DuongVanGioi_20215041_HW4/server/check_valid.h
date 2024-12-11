#ifndef VALIDATION_H
#define VALIDATION_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <arpa/inet.h> // For inet_pton

// Function to validate username
int is_valid_username(const char *username);

// Function to validate email
int is_valid_email(const char *email);

// Function to validate phone number
int is_valid_phone(const char *phone);

// Function to validate password
int is_valid_password(const char *password);

// Function to validate IPv4 address
int is_valid_ipv4(const char *ip);

// Function to validate domain name
int is_valid_domain(const char *domain);

// Function to validate homepage (either an IPv4 or a domain)
int is_valid_homepage(const char *homepage);

#endif // VALIDATION_H
