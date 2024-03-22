#include <stdio.h>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib")

int main(int argc, char* argv[]) {
    // 初始化Winsock
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("WSAStartup failed.\n");
        return 1;
    }

    // 用法说明
    if (argc != 3) {
        printf("Usage: %s [ip] [port]\n", argv[0]);
        WSACleanup();
        return 1;
    }

    // 创建套接字
    SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock == INVALID_SOCKET) {
        printf("socket creation failed.\n");
        WSACleanup();
        return 1;
    }

    // 设置服务器地址
    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr(argv[1]);
    server.sin_port = htons(atoi(argv[2]));

    // 连接服务器
    if (connect(sock, (struct sockaddr*)&server, sizeof(server)) == SOCKET_ERROR) {
        printf("connection failed.\n");
        closesocket(sock);
        WSACleanup();
        return 1;
    }

    printf("connected to the server.\n");

    // 通信循环
    char buf[128];
    while (1) {
        printf("please enter: ");
        fgets(buf, sizeof(buf), stdin);

        // 去除换行符
        size_t len = strlen(buf);
        if (len > 0 && buf[len - 1] == '\n') {
            buf[len - 1] = '\0';
        }

        // 发送消息到服务器
        send(sock, buf, strlen(buf), 0);
        printf("waiting for response...\n");

        // 接收服务器响应
        int bytesReceived = recv(sock, buf, sizeof(buf), 0);
        if (bytesReceived > 0) {
            buf[bytesReceived] = '\0';
            printf("server says: %s\n", buf);
        }

        // 检查是否需要退出
        if (strcmp(buf, "quit") == 0) {
            printf("client is quitting.\n");
            break;
        }
    }

    // 关闭套接字并清理Winsock
    closesocket(sock);
    WSACleanup();

    return 0;
}
