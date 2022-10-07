#pragma once
#ifndef CLIENT_H
#define CLIENT_H
#include <iostream>
#include "CommonValues.h"

class GameHandler {
private:

public:
    int sockfd;
    STATE my_field[10][10];
    STATE opponent_field[10][10];
    int ships_position[10][4];
    bool connect();
    bool setInitShips();
    bool sendInitShips();
    bool sendTarget();
    bool sendSignal(SIGNAL_TOSERVER signal);
    bool getMyField();
    bool getOpponentField();
    SIGNAL_TOCLIENT getSignal();
    void showFields();
};

class Window {
public:
    bool createWindow();
    bool showWindow();
    bool updateWindow(STATE (&field)[10][10]);
    bool closeWindow();
};

#endif