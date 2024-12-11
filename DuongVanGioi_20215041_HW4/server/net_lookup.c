#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <ctype.h>
#include <stdbool.h>

bool isIPv4(const char *str) {
    int num, dots = 0;
    const char *ptr = str;
    char temp[4];  // Sử dụng để lưu giá trị từng phần số trong IPv4
    int temp_index = 0;

    if (str == NULL)
        return 0;

    while (*ptr) {
        // Nếu gặp dấu chấm, kiểm tri giá trị của phần trước đó
        if (*ptr == '.') {
            if (temp_index == 0)  // Không có số nào trước dấu chấm
                return 0;
            temp[temp_index] = '\0';  // Kết thúc chuỗi số
            num = atoi(temp);  // Chuyển thành số nguyên
            if (num < 0 || num > 255)  // Kiểm tra giới hạn số
                return 0;

            dots++;  // Đếm số dấu chấm
            temp_index = 0;  // Reset chỉ số cho phần tiếp theo
        } else if (isdigit(*ptr)) {
            // Lưu từng ký tự số vào `temp` để kiểm tra sau
            if (temp_index >= 3)  // Mỗi phần chỉ có thể có tối đa 3 chữ số
                return 0;
            temp[temp_index++] = *ptr;
        } else {
            return 0;  // Ký tự không hợp lệ
        }
        ptr++;
    }

    // Xử lý phần cuối cùng sau dấu chấm cuối cùng
    if (temp_index == 0)  // Đảm bảo có số sau dấu chấm cuối cùng
        return 0;
    temp[temp_index] = '\0';
    num = atoi(temp);
    if (num < 0 || num > 255)
        return 0;

    // Địa chỉ IPv4 phải có đúng 3 dấu chấm và 4 phần số
    return dots == 3;
}


// Kiểm tra xem một chuỗi có phải là domain name không
bool isDomain(const char *str) {
    int len = strlen(str);

    // Kiểm tra độ dài tối thiểu và tối đa của domain
    if (len < 3 || len > 255)
        return 0;

    // Kiểm tra từng ký tự trong domain
    for (int i = 0; i < len; i++) {
        if (!(isalnum(str[i]) || str[i] == '-' || str[i] == '.')) {
            return 0;
        }
    }

    // Kiểm tra xem domain có dấu chấm không
    if (strchr(str, '.') == NULL)
        return 0;

    // Kiểm tra xem domain không bắt đầu hoặc kết thúc bằng dấu '-' hoặc '.'
    if (str[0] == '-' || str[0] == '.' || str[len - 1] == '-' || str[len - 1] == '.')
        return 0;

    return 1;
}
char* lookup_ip(const char *ip) {
    struct sockaddr_in sa;
    char host[1024];
    char service[20];

    sa.sin_family = AF_INET;
    
    inet_pton(AF_INET, ip, &sa.sin_addr);
    int res = getnameinfo((struct sockaddr*)&sa, sizeof(sa), host, sizeof(host), service, sizeof(service), 0);
    if (res) {
        return strdup("No information found");
    } else {
        return strdup(host);
    }
}

char* lookup_domain(const char *domain) {
    struct addrinfo hints, *res;
    int status;
    char ipstr[INET6_ADDRSTRLEN];
    char *result = NULL;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    if ((status = getaddrinfo(domain, NULL, &hints, &res)) != 0) {
        return strdup("No information found");
    }

    void *addr = &((struct sockaddr_in *)res->ai_addr)->sin_addr;
    inet_ntop(res->ai_family, addr, ipstr, sizeof ipstr);

    result = strdup(ipstr);
    freeaddrinfo(res);
    return result;
}
