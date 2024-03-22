#include <stdio.h>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib")

// 得到监听套接字函数
SOCKET server_sock(char* ip, int port) {
    // 初始化Winsock
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("WSAStartup failed.\n");
        return INVALID_SOCKET;
    }

    // 创建套接字
    SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock == INVALID_SOCKET) {
        printf("socket creation failed.\n");
        WSACleanup();
        return INVALID_SOCKET;
    }

    // 设置服务器地址
    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr(ip);
    server.sin_port = htons(port);

    // 绑定套接字
    if (bind(sock, (struct sockaddr*)&server, sizeof(server)) == SOCKET_ERROR) {
        printf("bind error\n");
        closesocket(sock);
        WSACleanup();
        return INVALID_SOCKET;
    }

    // 监听连接
    if (listen(sock, 5) == SOCKET_ERROR) {
        printf("listen error\n");
        closesocket(sock);
        WSACleanup();
        return INVALID_SOCKET;
    }

    return sock; // 返回监听套接字
}

// 客户端与服务器端建立连接之后的通信函数
void server_work(SOCKET client_sock, char* ip, int port) {
    char buf[128];
    while (1) {
        buf[0] = 0;
        // 从客户端接收信息
        int bytesReceived = recv(client_sock, buf, sizeof(buf), 0);
        if (bytesReceived > 0) {
            buf[bytesReceived] = '\0';
            printf("ip: %s, port: %d says: %s\n", ip, port, buf);
        } else if (bytesReceived == 0) {
            printf("ip: %s, port: %d quit\n", ip, port);
            break;
        } else {
            printf("read error\n");
            break;
        }

        // 服务器向客户端发送消息
        printf("please enter: ");
        fgets(buf, sizeof(buf), stdin);
        send(client_sock, buf, strlen(buf), 0);
        printf("waiting ...\n");
    }
}

int main(int argc, char* argv[]) {
    // 如果命令行参数个数不为3，弹出用法说明
    if (argc != 3) {
        printf("Usage: %s [ip] [port]\n", argv[0]);
        return 1;
    }

    // 打开网卡文件，将其绑定到指定的端口号和IP地址，并使其处于监听状态
    SOCKET listen_sock = server_sock(argv[1], atoi(argv[2]));
    if (listen_sock == INVALID_SOCKET) {
        printf("Failed to create listening socket.\n");
        return 1;
    }
    printf("Bind and listen success, waiting for connections...\n");

    // 绑定并监听成功后，双方开始通信
    struct sockaddr_in client;
    int client_len = sizeof(client);
    while (1) {
        // 服务器端调用该函数阻塞等待客户端发来连接请求
        // 如果连接建立成功之后，该函数接受客户端的连接请求
        SOCKET client_sock = accept(listen_sock, (struct sockaddr*)&client, &client_len);
        if (client_sock == INVALID_SOCKET) {
            printf("accept error\n");
            continue;
        }

        char ip_buf[24];
        inet_ntop(AF_INET, &client.sin_addr, ip_buf, sizeof(ip_buf));
        int port = ntohs(client.sin_port);
        printf("Connected, IP: %s, Port: %d\n", ip_buf, port);

        // 双方开始通信
        server_work(client_sock, ip_buf, port);

        // 关闭客户端套接字
        closesocket(client_sock);
    }

    // 关闭监听套接字并清理Winsock
    closesocket(listen_sock);
    WSACleanup();
    return 0;
}
