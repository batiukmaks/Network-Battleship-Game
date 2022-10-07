#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "Client.h"
#include "SocketManager.h"

void print_field(STATE field[10][10]) {
    std::cout << "  ";
    for (int i = 0; i < 10; i++) std::cout << i << " ";
    std::cout << std::endl;
    for (int i = 0; i < 10; i++) {
        std::cout << i << " ";
        for (int j = 0; j < 10; j++) {
            std::cout << STATE_CHAR[field[i][j]] << " ";
        }
        std::cout << std::endl;
    }
}

bool valid_ship(STATE field[10][10], int ship_size, int hx, int hy, int tx, int ty) {
    if (!((hx == tx && std::abs(hy - ty) + 1 == ship_size) || (hy == ty && std::abs(hx - tx) + 1 == ship_size)))
        return false;
    for (int x = std::min(hx, tx); x <= std::max(hx, tx); x++)
        for (int y = std::min(hy, ty); y <= std::max(hy, ty); y++)
            if (field[x][y] != (STATE)WATER)
                return false;
    return true;
}

bool GameHandler::connect() {
    SocketManager socket_manager;
    struct sockaddr_in sa;
    if (socket_manager.create_socket(sockfd) == -1)
        return false;
    if (socket_manager.create_sockaddr_in(sa, SOCK_INITIAL_PORT, SOCK_IP) == -1)
        return false;
    if (socket_manager.connect_socket(sockfd, (sockaddr*)&sa, sizeof(sa)) == -1)
        return false;

    std::cout << "Connected to server\n";
    return true;
};

bool GameHandler::setInitShips() {
    for (int i = 0; i < 10; i++)
        for (int j = 0; j < 10; j++)
            my_field[i][j] = (STATE)WATER;

    std::cout << "Set ships:\n";
    for (int ship_size = 1, size_variations = 4, ship_itr = 0; ship_size <= size_variations; ship_size++) {
        for (int itr = 0; itr < size_variations - ship_size + 1; itr++) {
            bool valid = true;
            int hx, hy, tx, ty;
            do {
                // system("clear");
                if (!valid) std::cout << "Invalid input! Try again.\n";
                print_field(my_field);
                valid = true;
                std::cout << "Set head and tail positions for ship of size " << ship_size << " (head_x head_y tail_x tail_y, 0...9): ";
                std::cin >> hx >> hy >> tx >> ty;
                if (!valid_ship(my_field, ship_size, hx, hy, tx, ty))
                    valid = false;
            }
            while (!valid);

            ships_position[ship_itr][0] = std::min(hx, tx);
            ships_position[ship_itr][1] = std::min(hy, ty);
            ships_position[ship_itr][2] = std::max(hx, tx);
            ships_position[ship_itr][3] = std::max(hy, ty);
            ship_itr++;
            for (int x = std::min(hx, tx); x <= std::max(hx, tx); x++)
                for (int y = std::min(hy, ty); y <= std::max(hy, ty); y++)
                    my_field[x][y] = (STATE)INTACT;
        }
    }
    std::cout << "Set field fully.\n";
    return true;
}

bool GameHandler::sendInitShips() {
    char buffer[10 * 4 + 1];
    for (int i = 0; i < 10; i++)
        for (int j = 0; j < 4; j++)
            buffer[4 * i + j] = ships_position[i][j] + '0';
    buffer[10 * 4] = '\0';
    send(sockfd, buffer, sizeof(buffer), 0);
    std::cout << "Sent init ships.\n";
    return true;
};

bool GameHandler::sendTarget() {
    int x, y;
    char buffer[3];
    std::cout << "Choose the target (x y): ";
    std::cin >> x >> y;
    buffer[0] = x + '0', buffer[1] = y + '0', buffer[2] = '\0';
    send(sockfd, buffer, sizeof(buffer), 0);
    std::cout << "Sent target.\n";
    return true;
};

bool GameHandler::getMyField() {
    char buffer[10 * 10 + 1];
    recv(sockfd, buffer, sizeof(buffer), 0);
    for (int i = 0; i < 10 * 10; i++)
        my_field[i / 10][i % 10] = (STATE)(buffer[i] - '0');
    std::cout << "Received my field.\n";
    return true;
}

bool GameHandler::getOpponentField() {
    char buffer[10 * 10 + 1];
    recv(sockfd, buffer, sizeof(buffer), 0);
    for (int i = 0; i < 10 * 10; i++)
        opponent_field[i / 10][i % 10] = (STATE)(buffer[i] - '0');
    std::cout << "Received opponent's field.\n";
    return true;
}

SIGNAL_TOCLIENT GameHandler::getSignal() {
    char buffer[100];
    recv(sockfd, buffer, sizeof(buffer), 0);
    std::cout << "Received signal: " << buffer << "\n";
    return SIGNAL_TOCLIENT_FROM_STR[buffer];
}

bool GameHandler::sendSignal(SIGNAL_TOSERVER signal) {
    char buffer[100];
    int size = 100;
    for (auto itr = SIGNAL_TOSERVER_FROM_STR.begin(); itr != SIGNAL_TOSERVER_FROM_STR.end(); itr++) {
        if (itr->second == signal) {
            int size = 0;
            const char *p = &(itr->first[0]);
            while (*p != '\0')
                buffer[size++] = *(p++);
            buffer[size++] = '\0';
            break;
        }
    }
    send(sockfd, buffer, size, 0);
    std::cout << "Sent signal:" << buffer << "\n";
    return true;
}

void GameHandler::showFields() {
    std::cout << "##### Your field:\n";
    print_field(my_field);
    std::cout << "\n\n##### Opponent's field:\n";
    print_field(opponent_field);
}