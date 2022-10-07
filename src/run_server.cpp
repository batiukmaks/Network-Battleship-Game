#include "Server.h"

void process_move(Player* attacker, Player* defender, Judge judge) {
    std::pair<int, int> target;
    JUDGE_CONCLUSION concl;
    do {
        attacker->sendSignal((SIGNAL_TOCLIENT)RECV_FIELDS);
        attacker->sendPlayerField();
        attacker->sendOpponentField(defender->field);
        defender->sendSignal((SIGNAL_TOCLIENT)RECV_FIELDS);
        defender->sendPlayerField();
        defender->sendOpponentField(attacker->field);
        defender->sendSignal((SIGNAL_TOCLIENT)OPPONENT_MOVE);
        do {
            attacker->sendSignal((SIGNAL_TOCLIENT)GET_TARGET);
            target = attacker->getTarget(defender->field);
            concl = judge.handleShot(attacker, defender, target);
        }
        while (concl == (JUDGE_CONCLUSION)INVALID_TARGET);
    }
    while (concl == (JUDGE_CONCLUSION)HITTED);
    attacker->sendSignal((SIGNAL_TOCLIENT)RECV_FIELDS);
    attacker->sendPlayerField();
    attacker->sendOpponentField(defender->field);
    if (concl == DESTROYED_ALL)
        attacker->sendSignal((SIGNAL_TOCLIENT)NO_MORE_SHIPS);
}

int main() {
    Server server;
    Player *p1, *p2;
    Judge judge;
    p1 = new ClientPlayer;
    server.prepareToConnect();
    p1->connect(server.sockfd);
    if (p1->getSignal() == MULTIPLAYER_MODE)
        p2 = new ClientPlayer;
    else 
        p2 = new BotPlayer;
    p1->getInitShips();

    p2->connect(server.sockfd);
    p2->getSignal();
    p2->getInitShips();

    GAME_RESULT gr = UNFINISHED_GAME;
    while (gr == UNFINISHED_GAME) {
        std::cout << "\nPlayer1:\n";
        process_move(p1, p2, judge);
        std::cout << "\nPlayer2:\n";
        process_move(p2, p1, judge);
        gr = judge.isWinner(p1, p2);
    }
    if (gr == WON) {
        p1->sendSignal((SIGNAL_TOCLIENT)WIN);
        p2->sendSignal((SIGNAL_TOCLIENT)LOSE);
    }
    else if (gr == LOST) {
        p1->sendSignal((SIGNAL_TOCLIENT)LOSE);
        p2->sendSignal((SIGNAL_TOCLIENT)WIN);
    }
    else if (gr == TIE) {
        p1->sendSignal((SIGNAL_TOCLIENT)TIE_RES);
        p2->sendSignal((SIGNAL_TOCLIENT)TIE_RES);
    }
}

