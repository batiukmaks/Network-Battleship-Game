#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cmath>
#include <string.h>
#include <stdlib.h>
#include <time.h> 
#include "Server.h"
#include "SocketManager.h"

bool Server::prepareToConnect() {
    SocketManager socket_manager;
    struct sockaddr_in sa;
    if (socket_manager.create_socket(sockfd) == -1)
        return false;
    if (socket_manager.create_sockaddr_in(sa, SOCK_INITIAL_PORT, SOCK_IP) == -1)
        return false;
    if (socket_manager.bind_socket(sockfd, (sockaddr*)&sa, sizeof(sa)) == -1)
        return false;
    if (socket_manager.listen_socket(sockfd, MAX_CLIENTS) == -1)
        return false;
    return true;
}

bool ClientPlayer::connect(int& server_sockfd) {
    sockfd = server_sockfd;
    SocketManager socket_manager;
    struct sockaddr_in cliaddr;
    int len = sizeof(cliaddr);
    socket_manager.accept_socket(server_sockfd, clientfd, (sockaddr*)&cliaddr, (unsigned int*)&len);
    std::cout << "Accepted the client\n";
    return true;
}

bool ClientPlayer::getInitShips() {
    for (int i = 0; i < 10; i++)
        for (int j = 0; j < 10; j++)
            field[i][j] = (STATE)WATER;

    char buffer[10 * 4 + 1];
    recv(clientfd, buffer, sizeof(buffer), 0);
    for (int i = 0; i < 10 * 4; i++)
        ships_position[i / 4][i % 4] = buffer[i] - '0';
    for (int i = 0; i < 10; i++)
        for (int x = std::min(ships_position[i][0], ships_position[i][2]); x <= std::max(ships_position[i][0], ships_position[i][2]); x++)
            for (int y = std::min(ships_position[i][1], ships_position[i][3]); y <= std::max(ships_position[i][1], ships_position[i][3]); y++)
                field[x][y] = (STATE)INTACT;

    std::cout << "Recieved field.\n";
    return true;
}

std::pair<int, int> ClientPlayer::getTarget(STATE opp_field[10][10]) {
    char buffer[3];
    recv(clientfd, buffer, sizeof(buffer), 0);
    std::cout << "Target field.\n";
    return { buffer[0] - '0', buffer[1] - '0' };
}

bool ClientPlayer::disconnect() {
    close(clientfd);
    return true;
}

bool ClientPlayer::sendPlayerField() {
    char buffer[10 * 10 + 1];
    for (int i = 0; i < 10; i++)
        for (int j = 0; j < 10; j++)
            buffer[10 * i + j] = (char)((int)field[i][j] + '0');
    buffer[10 * 10] = '\0';
    send(clientfd, buffer, sizeof(buffer), 0);
    std::cout << "Sent player field.\n";
    return true;
}

bool ClientPlayer::sendOpponentField(STATE opp_field[10][10]) {
    char buffer[10 * 10 + 1];
    STATE state;
    char ch;
    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 10; j++) {
            state = opp_field[i][j];
            if (state == INJURED)
                ch = (int)INJURED + '0';
            else if (state == DESTROYED)
                ch = (int)DESTROYED + '0';
            else if (state == MISS)
                ch = (int)MISS + '0';
            else
                ch = (int)UNKNOWN + '0';
            buffer[10 * i + j] = ch;
        }
    }
    buffer[10 * 10] = '\0';
    send(clientfd, buffer, sizeof(buffer), 0);
    std::cout << "Sent opponent field.\n";
    return true;
}

bool ClientPlayer::updateState(int, int, STATE) {

    return true;
}

bool ClientPlayer::sendSignal(SIGNAL_TOCLIENT signal) {
    char buffer[100];
    int size = 100;
    for (auto itr = SIGNAL_TOCLIENT_FROM_STR.begin(); itr != SIGNAL_TOCLIENT_FROM_STR.end(); itr++) {
        if (itr->second == signal) {
            int size = 0;
            const char* p = &(itr->first[0]);
            while (*p != '\0')
                buffer[size++] = *(p++);
            buffer[size++] = '\0';
            break;
        }
    }
    send(clientfd, buffer, size, 0);
    std::cout << "Sent signal:" << buffer << "\n";
    return true;
}

SIGNAL_TOSERVER Player::getSignal() {
    char buffer[100];
    recv(clientfd, buffer, sizeof(buffer), 0);
    std::cout << "Received signal: " << buffer << "\n";
    return SIGNAL_TOSERVER_FROM_STR[buffer];
}

double distance(int x1, int y1, int x2, int y2) {
    return sqrt(pow(abs(x1 - x2), 2) + pow(abs(y1 - y2), 2));
}

int ship_number(int ships[10][4], int x, int y) {
    for (int i = 0; i < 10; i++) {
        int hx = ships[i][0], hy = ships[i][1], tx = ships[i][2], ty = ships[i][3];
        if (distance(hx, hy, x, y) + distance(x, y, tx, ty) == distance(hx, hy, tx, ty))
            return i;
    }
    return -1;
}

JUDGE_CONCLUSION Judge::handleShot(Player* attacker, Player* defender, std::pair<int, int> target) {
    if (defender->field[target.first][target.second] != INTACT && defender->field[target.first][target.second] != WATER)
        return INVALID_TARGET;

    JUDGE_CONCLUSION conclusion = INVALID_TARGET;
    if (defender->field[target.first][target.second] == INTACT) {
        defender->field[target.first][target.second] = INJURED;
        int ship = ship_number(defender->ships_position, target.first, target.second);
        bool isdestroyed = true;
        for (int x = defender->ships_position[ship][0]; x <= defender->ships_position[ship][2]; x++)
            for (int y = defender->ships_position[ship][1]; y <= defender->ships_position[ship][3]; y++)
                if (defender->field[x][y] == INTACT) isdestroyed = false;
        if (isdestroyed)
            for (int x = defender->ships_position[ship][0]; x <= defender->ships_position[ship][2]; x++)
                for (int y = defender->ships_position[ship][1]; y <= defender->ships_position[ship][3]; y++)
                    defender->field[x][y] = DESTROYED;
        conclusion = HITTED;
    } else {
        defender->field[target.first][target.second] = (STATE)MISS;
        conclusion = MISSED;
    }
    if (conclusion == HITTED) {
        bool destroyed_all = true;
        for (int i = 0; i < 10; i++)
            for (int j = 0; j < 10; j++)
                if (defender->field[i][j] == INTACT || defender->field[i][j] == INJURED)
                    destroyed_all = false;
        if (destroyed_all)
            conclusion = DESTROYED_ALL;
    }
    return conclusion;
}

GAME_RESULT Judge::isWinner(Player* player, Player* opponent) {
    int notdestroyed_player = 0, notdestroyed_opponent = 0;
    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 10; j++) {
            if (player->field[i][j] == INTACT)
                notdestroyed_player++;
            if (opponent->field[i][j] == INTACT)
                notdestroyed_opponent++;
        }
    }
    if (notdestroyed_player == 0 && notdestroyed_opponent == 0)
        return TIE;
    else if (notdestroyed_opponent == 0)
        return WON;
    else if (notdestroyed_player == 0)
        return LOST;

    return UNFINISHED_GAME;
}



// Bot
bool BotPlayer::connect(int& server_sockfd) {
    return true;
}

bool BotPlayer::getInitShips() {
    int hx, hy, tx, ty, direction;
    bool set_ship;
    std::srand(time(0));
    for (int i = 0; i < 10; i++)
        for (int j = 0; j < 10; j++)
            field[i][j] = (STATE)WATER;
    for (int ship_size = 1, size_variations = 4, ship_itr = 0; ship_size <= size_variations; ship_size++) {
        for (int itr = 0; itr < size_variations - ship_size + 1; itr++) {
            set_ship = false;
            while (!set_ship) {
                hx = rand() % 10, hy = rand() % 10, direction = rand() % 4;
                if (direction == 0)
                    tx = hx + ship_size - 1, ty = hy;
                else if (direction == 1)
                    tx = hx - ship_size + 1, ty = hy;
                else if (direction == 2)
                    tx = hx, ty = hy + ship_size - 1;
                else if (direction == 3)
                    tx = hx, ty = hy - ship_size + 1;

                if (!(0 <= tx && tx <= 9 && 0 <= ty && ty <= 9))
                    continue;

                set_ship = true;
                for (int x = std::min(hx, tx); x <= std::max(hx, tx); x++)
                    for (int y = std::min(hy, ty); y <= std::max(hy, ty); y++)
                        if (field[x][y] != WATER) set_ship = false;
                if (!set_ship)
                    continue;

                ships_position[ship_itr][0] = std::min(hx, tx);
                ships_position[ship_itr][1] = std::min(hy, ty);
                ships_position[ship_itr][2] = std::max(hx, tx);
                ships_position[ship_itr][3] = std::max(hy, ty);
                for (int x = std::min(hx, tx); x <= std::max(hx, tx); x++)
                    for (int y = std::min(hy, ty); y <= std::max(hy, ty); y++)
                        field[x][y] = (STATE)INTACT;
            }
            ship_itr++;
        }
    }
    return true;
}

std::pair<int, int> BotPlayer::getTarget(STATE opp_field[10][10]) {
    STATE unknown_opp_field[10][10];
    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 10; j++) {
            STATE state = opp_field[i][j];
            if (!(state == INJURED || state == DESTROYED || state == MISS))
                unknown_opp_field[i][j] = UNKNOWN;
            else 
                unknown_opp_field[i][j] = state;
        }
    }
    
    std::pair<int, int> p = { -1, -1 };
    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 10; j++) {
            if (unknown_opp_field[i][j] == INJURED) {
                if (0 < j && j < 9 && unknown_opp_field[i][j + 1] == INJURED && unknown_opp_field[i][j - 1] == UNKNOWN) {
                    return { i, j - 1 };
                } else if (0 < i && i < 9 && unknown_opp_field[i + 1][j] == INJURED && unknown_opp_field[i - 1][j] == UNKNOWN) {
                    return { i - 1, j };
                } else {
                    std::pair<int, int> pairs[] = { {i + 1, j}, {i - 1, j}, {i, j + 1}, {i, j - 1} };
                    bool is_anyunknown = false;
                    for (std::pair<int, int> pair_direction : pairs)
                        if (0 <= pair_direction.first && pair_direction.first < 10 && 0 <= pair_direction.second && pair_direction.second < 10 && unknown_opp_field[pair_direction.first][pair_direction.second] == UNKNOWN)
                            is_anyunknown = true;


                    while (is_anyunknown) {
                        int direction = rand() % 4;
                        p = pairs[direction];
                        if (0 <= p.first && p.first < 10 && 0 <= p.second && p.second < 10 && unknown_opp_field[p.first][p.second] == UNKNOWN)
                            return p;
                    }
                }
            }
        }
    }
    p = { rand() % 10, rand() % 10 };
    return p;
}

bool BotPlayer::disconnect() {
    return true;
}

bool BotPlayer::sendPlayerField() {
    return true;
}

bool BotPlayer::sendOpponentField(STATE opp_field[10][10]) {
    return true;
}

bool BotPlayer::updateState(int row, int col, STATE state) {
    return true;
}

bool BotPlayer::sendSignal(SIGNAL_TOCLIENT signal) {
    return true;
}