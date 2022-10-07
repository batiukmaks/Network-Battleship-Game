#include <sys/socket.h>
#include <errno.h>
#include <cstring>
#include <arpa/inet.h>
#include <unistd.h>
#include <string>
#include "SocketManager.h"

int SocketManager::create_socket(int& sockfd) {
    sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sockfd < 0) {
        int err = errno;
        printf("Error. Socket: strerror=%d: %s\n", err, strerror(err));
        return -1;
    }
    else {
        printf("Socket successfully created.\n");
    }
    return 0;
}

int SocketManager::create_sockaddr_in(struct sockaddr_in &sa, const int port, const char* ip) {
    memset(&sa, 0, sizeof(sa));
    sa.sin_family = AF_INET;
    sa.sin_port = htons(port);
    inet_pton(AF_INET, ip, &sa.sin_addr);
    return 0;
}

int SocketManager::bind_socket(int &sockfd, sockaddr* sa, int size) {
    if (bind(sockfd, sa, size) != 0){
        int err = errno;
        printf("Error. Bind: strerror=%d: %s\n", err, strerror(err));
        return 1;
    }
    else {
        printf("Socket successfully binded.\n");
    }
    return 0;
}

int SocketManager::listen_socket(int& sockfd, int clients_count) {
    if (listen(sockfd, clients_count)){
        int err = errno;
        printf("Error. Listen: strerror=%d: %s\n", err, strerror(err));
        return -1;
    }
    else {
        printf("Socket successfully marked as a passive socket. (listen())\n");
    }
    return 0;
}

int SocketManager::accept_socket(int &sockfd, int &clientfd, sockaddr* client, unsigned int *len) {
    clientfd = accept(sockfd, client, len);
    if (clientfd < 0){
        int err = errno;
        printf("Error. Accept: strerror=%d: %s\n", err, strerror(err));
        return -1;
    }
    else {
        printf("Client successfully accepted.\n");
    }  
   return 0;
}

int SocketManager::reaccept(int &sockfd, int &clientfd, sockaddr_in &client, sockaddr_in &sa, unsigned int *len, int clients_count) {
    close(clientfd);
    close(sockfd);
    SocketManager::create_socket(sockfd);
    sa.sin_port = htons(ntohs(sa.sin_port) + 5);
    if (SocketManager::bind_socket(sockfd, (sockaddr*)&sa, sizeof(sa)) == -1)
        return -1;
    
    if (SocketManager::listen_socket(sockfd, clients_count) == -1)
        exit(1);

    if (SocketManager::accept_socket(sockfd, clientfd, (sockaddr*)&client, len) == -1)
        return -1;

    return 0;
}

int SocketManager::connect_socket(int &sockfd, sockaddr* sa, int size) {
    if (connect(sockfd, sa, size) != 0) {
        int err = errno;
        printf("Error. Connect: strerror=%d: %s\n", err, strerror(err));
        return -1;
    }
    else {
        printf("Client successfully connected.\n");
    }
    return 0;
}

int SocketManager::reconnect(int &sockfd, sockaddr_in &sa) {
    close(sockfd);
    if (SocketManager::create_socket(sockfd))
        return 1;
    sa.sin_port = htons(ntohs(sa.sin_port) + 5);
    sleep(2);
    if (SocketManager::connect_socket(sockfd, (sockaddr*)&sa, sizeof(sa)))
        return 1;
    return 0;
}
