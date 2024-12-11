#ifndef NETWORK_H
#define NETWORK_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include "user.h"
#include "check_valid.h"
#define BUFF_SIZE 1024
#define MAX_BUFFER 1024
#define MAX_CLIENT 3
typedef struct {
    int sockfd;
    pid_t pid;
    char username[100];
} Session;

typedef struct {
    char username[100];
    Session sessions[MAX_CLIENT];  // Mảng chứa các session của người dùng
    int session_count;  // Số lượng session của người dùng
} ClientSession;

void serverProcess(int sockfd, ClientSession *clientSession, int *clientCount);
#endif
