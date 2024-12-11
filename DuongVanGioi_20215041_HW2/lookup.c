#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <netdb.h>
#include"check_valid.h"

void lookup_ip(const char *ip) {
    struct sockaddr_in sa;
    char host[1024];
    char service[20];
    if(!is_valid_ipv4(ip)){
        printf("Invalid IP\n");
        return;
    }

    sa.sin_family = AF_INET;
    inet_pton(AF_INET, ip, &sa.sin_addr);
    int res = getnameinfo((struct sockaddr*)&sa, sizeof(sa), host, sizeof(host), service, sizeof(service), 0);
    if (res) {
        printf("No information found\n");
    } else {
        printf("Main name: %s\n", host);
        printf("Alternate names:\n");
    }
}

void lookup_domain(const char *domain) {
    if(!is_valid_domain(domain)){
        printf("Invalid domain\n");
        return;
    }
    struct addrinfo hints, *res, *p;
    int status;
    char ipstr[INET6_ADDRSTRLEN];

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    if ((status = getaddrinfo(domain, NULL, &hints, &res)) != 0) {
        printf("No information found\n");
        return;
    } 

    printf("Main IP: %s\n", inet_ntop(res->ai_family, &((struct sockaddr_in *)res->ai_addr)->sin_addr, ipstr, sizeof ipstr));
    printf("Alternate IP:\n");

    for (p = res->ai_next; p != NULL; p = p->ai_next) {
        void *addr;
        if (p->ai_family == AF_INET) {
            addr = &((struct sockaddr_in *)p->ai_addr)->sin_addr;
        } else {
            addr = &((struct sockaddr_in6 *)p->ai_addr)->sin6_addr;
        }
        inet_ntop(p->ai_family, addr, ipstr, sizeof ipstr);
        printf("%s\n", ipstr);
    }

    freeaddrinfo(res);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s option parameter\n", argv[0]);
        return 1;
    }

    int option = atoi(argv[1]);
    char *parameter = argv[2];

    struct sockaddr_in sa;
    int is_ip = inet_pton(AF_INET, parameter, &(sa.sin_addr));

    if (option == 1 && is_ip == 1) {
        lookup_ip(parameter);
    } else if (option == 2 && is_ip == 0) {
        lookup_domain(parameter);
    } else {
        printf("Invalid option\n");
    }

    return 0;
}