#ifndef SOCKETMANAGER_H
#define SOCKETMANAGER_H

class SocketManager {
public:
    int create_socket(int& sockfd);
    int create_sockaddr_in(struct sockaddr_in &sa, const int port, const char* ip);
    int bind_socket(int &sockfd, sockaddr* sa, int size);
    int listen_socket(int& sockfd, int clients_count);
    int accept_socket(int &sockfd, int &clientfd, sockaddr* client, unsigned int *len);
    int reaccept(int &sockfd, int &clientfd, sockaddr_in &client, sockaddr_in &sa, unsigned int *len, int clients_count);
    int connect_socket(int &sockfd, sockaddr* sa, int size);
    int reconnect(int &sockfd, sockaddr_in &sa);
};
#endif