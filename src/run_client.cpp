#include "Client.h"

int main() {
    GameHandler gh;
    if (!gh.connect()) {
        std::cout << "Cannot connect to the server.\n";
        return 0;
    }
    while (true) {
        char choice;
        std::cout << "Choose the game mode: Multiplayer [M] or Bot (computer) [B].\nChoice: ";
        std::cin >> choice;
        if (choice == 'M')
            gh.sendSignal(MULTIPLAYER_MODE);
        else if (choice == 'B')
            gh.sendSignal(BOT_MODE);
        if (choice == 'M' || choice == 'B')
            break;
    }
    gh.setInitShips();
    gh.sendInitShips();

    SIGNAL_TOCLIENT signal;
    while (signal != WIN && signal != LOSE && signal != TIE_RES) {
        signal = gh.getSignal();
        switch (signal) {
            case RECV_FIELDS:
                gh.getMyField();
                gh.getOpponentField();
                gh.showFields();
                break;
            case GET_TARGET:
                gh.sendTarget();
                break;
            case OPPONENT_MOVE:
                std::cout << "Opponent's move.\n";
                break;
            case NO_MORE_SHIPS:
                std::cout << "You destroyed all opponent's ships. Let's wait for the opponent's last moves.\n";
                break;
            case WIN:
                std::cout << "You won!\n";
                break;
            case LOSE:
                std::cout << "You lost!\n";
                break;
            case TIE_RES:
                std::cout << "TIE!\n";
                break;
        }
    }
    std::cout << "The game is over.\n";
}