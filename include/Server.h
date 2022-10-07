#pragma once
#ifndef SERVER_H
#define SERVER_H
#include <iostream>
#include "CommonValues.h"

class Server {
public:
    int sockfd;
    bool prepareToConnect();
};

class Player {
private:
    
public:
    int sockfd;
    int clientfd;
    STATE field[10][10];
    int ships_position[10][4];
    virtual bool connect(int &server_sockfd) = 0;
    virtual bool getInitShips() = 0;
    virtual std::pair<int, int> getTarget(STATE opp_field[10][10]) = 0;
    virtual bool disconnect() = 0;
    virtual bool sendPlayerField() = 0;
    virtual bool sendOpponentField(STATE opp_field[10][10]) = 0;
    virtual bool updateState(int row, int col, STATE state) = 0;
    virtual bool sendSignal(SIGNAL_TOCLIENT signal) = 0;
    SIGNAL_TOSERVER getSignal();
};

class ClientPlayer : public Player {
private:


public:
    virtual bool connect(int &server_sockfd) override;
    virtual bool getInitShips() override;
    virtual std::pair<int, int> getTarget(STATE opp_field[10][10]) override;
    virtual bool disconnect() override;
    virtual bool sendPlayerField() override;
    virtual bool sendOpponentField(STATE opp_field[10][10]) override;
    virtual bool updateState(int row, int col, STATE state) override;
    virtual bool sendSignal(SIGNAL_TOCLIENT signal) override;
};

class BotPlayer : public Player {
public:
    virtual bool connect(int &server_sockfd) override;
    virtual bool getInitShips() override;
    virtual std::pair<int, int> getTarget(STATE opp_field[10][10]) override;
    virtual bool disconnect() override;
    virtual bool sendPlayerField() override;
    virtual bool sendOpponentField(STATE opp_field[10][10]) override;
    virtual bool updateState(int row, int col, STATE state) override;
    virtual bool sendSignal(SIGNAL_TOCLIENT signal) override;
};

class Judge {
public:
    JUDGE_CONCLUSION handleShot(Player* attacker, Player* defender, std::pair<int, int> target);
    GAME_RESULT isWinner(Player* player, Player* opponent);
};


#endif