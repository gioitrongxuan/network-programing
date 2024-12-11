#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <pthread.h>
#include "network.h"
#include <sys/shm.h>
#include <errno.h>

#define BUFF_SIZE 1024
#define MAX_CLIENT 3
#define SHM_KEY 0x1234


ClientSession *clientSession;
int *clientCount;
int currentSession;

pthread_mutex_t client_mutex = PTHREAD_MUTEX_INITIALIZER;

// Hàm xử lý kết nối của client
void *client_handler(void *arg) {
    int new_socket = *((int *)arg);
    free(arg);  // Giải phóng bộ nhớ đã cấp phát cho socket

    // Giả sử currentUser là một biến toàn cục hoặc được truyền vào hàm
    char currentUser[100]; // Đây là biến giả lập để thay thế currentUser thực tế
    
    // Kiểm tra và thêm session mới vào người dùng
    pthread_mutex_lock(&client_mutex);
    int found = 0;
    for (int i = 0; i < *clientCount; i++) {
        if (strcmp(clientSession[i].username, currentUser) == 0) {
            // Người dùng đã có session, thêm session mới vào danh sách
            if (clientSession[i].session_count < MAX_CLIENT) {
                clientSession[i].sessions[clientSession[i].session_count].sockfd = new_socket;
                clientSession[i].sessions[clientSession[i].session_count].pid = getpid();
                strcpy(clientSession[i].sessions[clientSession[i].session_count].username, currentUser);
                clientSession[i].session_count++;
                found = 1;
            } else {
                printf("Max sessions reached for user %s\n", currentUser);
                close(new_socket);
                pthread_mutex_unlock(&client_mutex);
                return NULL;
            }
            break;
        }
    }

    if (!found) {
        // Nếu người dùng chưa có session nào, tạo mới
        if (*clientCount < MAX_CLIENT) {
            clientSession[*clientCount].session_count = 1;
            clientSession[*clientCount].sessions[0].sockfd = new_socket;
            clientSession[*clientCount].sessions[0].pid = getpid();
            strcpy(clientSession[*clientCount].username, currentUser);
            (*clientCount)++;
        } else {
            printf("Max clients reached\n");
            close(new_socket);
            pthread_mutex_unlock(&client_mutex);
            return NULL;
        }
    }
    pthread_mutex_unlock(&client_mutex);

    // Xử lý kết nối với client
    serverProcess(new_socket, clientSession, clientCount);

    // Cập nhật lại thông tin session khi client ngắt kết nối
    pthread_mutex_lock(&client_mutex);
    for (int i = 0; i < *clientCount; i++) {
        if (strcmp(clientSession[i].username, currentUser) == 0) {
            for (int j = 0; j < clientSession[i].session_count; j++) {
                if (clientSession[i].sessions[j].sockfd == new_socket) {
                    // Xoá session tương ứng
                    for (int k = j; k < clientSession[i].session_count - 1; k++) {
                        clientSession[i].sessions[k] = clientSession[i].sessions[k + 1];
                    }
                    clientSession[i].session_count--;
                    break;
                }
            }
            break;
        }
    }
    pthread_mutex_unlock(&client_mutex);

    close(new_socket); // Đóng socket sau khi xử lý xong
    return NULL;
}

int main(int argc, char *argv[]) {
    int sockfd, new_socket;
    struct sockaddr_in servaddr, cliaddr;

    // Đọc danh sách người dùng từ file
    readUsers();

    // Kiểm tra số lượng tham số dòng lệnh
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <port>\n", argv[0]);
        return 1;
    }

    // Chuyển port từ tham số dòng lệnh sang số nguyên
    int port = atoi(argv[1]);
    if (port <= 0 || port > 65535) {
        fprintf(stderr, "Error: Invalid port number. Must be between 1 and 65535.\n");
        return 1;
    }

    // Tính toán kích thước bộ nhớ cần thiết
    size_t shm_size = sizeof(ClientSession) * MAX_CLIENT + sizeof(int);
    printf("Shared memory size: %zu bytes\n", shm_size);

    // Tạo bộ nhớ chia sẻ
    int shmid = shmget(SHM_KEY, shm_size, IPC_CREAT | 0666);
    if (shmid < 0) {
        perror("shmget failed");
        fprintf(stderr, "shmget failed with errno = %d\n", errno); // Thêm thông báo lỗi chi tiết
        return 1;
    }

    // Ánh xạ bộ nhớ chia sẻ
    void *shm = shmat(shmid, NULL, 0);
    if (shm == (void *) -1) {
        perror("shmat failed");
        return 1;
    }

    clientSession = (ClientSession *) shm;
    clientCount = (int *) (shm + sizeof(ClientSession) * MAX_CLIENT);
    *clientCount = 0;

    // Bước 1: Tạo socket
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Error: ");
        return 1;
    }

    // Bước 2: Gán địa chỉ cho socket
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(port);

    if (bind(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0) {
        perror("Error: ");
        return 1;
    }

    // Bước 3: Lắng nghe kết nối
    if (listen(sockfd, 5) < 0) {
        perror("Error: connection refused");
        return 1;
    }
    printf("Server started on port %d.\n", port);

    while (1) {
        socklen_t addrlen = sizeof(cliaddr);
        // Bước 4: Chấp nhận kết nối từ client
        new_socket = accept(sockfd, (struct sockaddr *)&cliaddr, &addrlen);
        if (new_socket < 0) {
            perror("accept failed");
            continue;
        }

        // Tạo thread mới để xử lý client
        pthread_t tid;
        int *socket_ptr = malloc(sizeof(int));
        if (socket_ptr == NULL) {
            perror("malloc failed");
            close(new_socket);
            continue;
        }
        *socket_ptr = new_socket;

        if (pthread_create(&tid, NULL, client_handler, socket_ptr) != 0) {
            perror("pthread_create failed");
            free(socket_ptr);  // Giải phóng bộ nhớ khi không thể tạo thread
            close(new_socket);
        } else {
            pthread_detach(tid);  // Tách thread để không phải gọi pthread_join
        }
    }

    // Detach shared memory
    shmdt(shm);

    close(sockfd); // Đóng socket lắng nghe khi server kết thúc
    return 0;
}
