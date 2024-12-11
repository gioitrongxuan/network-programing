#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>
#include <sys/shm.h>
#include <errno.h>

#define BUFF_SIZE 1024
#define MAX_CLIENT 3
#define SHM_KEY 0x1234

ClientSession *clientSession;
int *clientCount;

pthread_mutex_t client_mutex = PTHREAD_MUTEX_INITIALIZER;

// Hàm xử lý từng client
void *client_handler(void *arg) {
    int new_socket = *((int *)arg);
    free(arg);  // Giải phóng bộ nhớ đã cấp phát cho socket

    char username[100];
    int authenticated = 0;

    // Bước 1: Đọc tên người dùng từ client
    if (recv(new_socket, username, sizeof(username), 0) <= 0) {
        perror("recv failed");
        close(new_socket);
        return NULL;
    }

    // Kiểm tra và thêm session mới vào người dùng
    pthread_mutex_lock(&client_mutex);
    int found = 0;
    for (int i = 0; i < *clientCount; i++) {
        if (strcmp(clientSession[i].username, username) == 0) {
            // Người dùng đã có session, thêm session mới vào danh sách
            if (clientSession[i].session_count < MAX_CLIENT) {
                clientSession[i].sessions[clientSession[i].session_count].sockfd = new_socket;
                clientSession[i].sessions[clientSession[i].session_count].pid = getpid();
                strcpy(clientSession[i].sessions[clientSession[i].session_count].username, username);
                clientSession[i].session_count++;
                found = 1;
                authenticated = 1;
            } else {
                char msg[] = "Max sessions reached for this user.\n";
                send(new_socket, msg, strlen(msg), 0);
                close(new_socket);
                pthread_mutex_unlock(&client_mutex);
                return NULL;
            }
            break;
        }
    }

    if (!found) {
        // Nếu người dùng chưa có session nào, tạo mới
        if (*clientCount >= MAX_CLIENT) {
            char msg[] = "Server is full. Try again later.\n";
            send(new_socket, msg, strlen(msg), 0);
            close(new_socket);
            pthread_mutex_unlock(&client_mutex);
            return NULL;
        }
        // Tạo session mới cho người dùng
        clientSession[*clientCount].session_count = 1;
        clientSession[*clientCount].sessions[0].sockfd = new_socket;
        clientSession[*clientCount].sessions[0].pid = getpid();
        strcpy(clientSession[*clientCount].username, username);
        (*clientCount)++;
        authenticated = 1;
    }

    pthread_mutex_unlock(&client_mutex);

    // Nếu không xác thực, thông báo lỗi
    if (!authenticated) {
        char msg[] = "Authentication failed.\n";
        send(new_socket, msg, strlen(msg), 0);
        close(new_socket);
        return NULL;
    }

    // Xử lý kết nối với client
    serverProcess(new_socket, clientSession, clientCount);

    // Cập nhật lại thông tin session khi client ngắt kết nối
    pthread_mutex_lock(&client_mutex);
    for (int i = 0; i < *clientCount; i++) {
        if (strcmp(clientSession[i].username, username) == 0) {
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
            // Nếu không còn session nào, xóa người dùng khỏi danh sách
            if (clientSession[i].session_count == 0) {
                for (int k = i; k < *clientCount - 1; k++) {
                    clientSession[k] = clientSession[k + 1];
                }
                (*clientCount)--;
            }
            break;
        }
    }
    pthread_mutex_unlock(&client_mutex);

    close(new_socket); // Đóng socket sau khi xử lý xong
    return NULL;
}

// Hàm main của server
int main(int argc, char *argv[]) {
    int sockfd, new_socket;
    struct sockaddr_in servaddr, cliaddr;

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
        return 1;
    }

    // Gắn kết bộ nhớ chia sẻ
    void *shm = shmat(shmid, NULL, 0);
    if (shm == (void *) -1) {
        perror("shmat failed");
        return 1;
    }

    // Gán con trỏ đến bộ nhớ chia sẻ
    clientSession = (ClientSession *) shm;
    clientCount = (int *) (shm + sizeof(ClientSession) * MAX_CLIENT);

    // Kiểm tra con trỏ bộ nhớ chia sẻ
    if (clientSession == NULL || clientCount == NULL) {
        fprintf(stderr, "Error: Failed to initialize shared memory for clientSession or clientCount.\n");
        shmdt(shm); // Ngắt kết nối bộ nhớ chia sẻ
        return 1;
    }

    // Khởi tạo số lượng client ban đầu
    *clientCount = 0;

    // Bước 1: Tạo socket
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        return 1;
    }

    // Bước 2: Gán địa chỉ cho socket
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(port);

    if (bind(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0) {
        perror("bind failed");
        return 1;
    }

    // Bước 3: Lắng nghe kết nối
    if (listen(sockfd, 5) < 0) {
        perror("listen failed");
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

        // Kiểm tra và tạo thread
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

    // Đóng socket khi server kết thúc
    close(sockfd);
    return 0;
}
