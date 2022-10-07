#pragma once
#ifndef COMMONVALUES_H
#define COMMONVALUES_H
#include <map>
#include <cstring>

// DESTROYED --> injured every part of ship
enum STATE {WATER, MISS, INTACT, INJURED, DESTROYED, UNKNOWN};
static std::map <STATE, const char*> STATE_CHAR = {
    {(STATE)WATER, "W"},
    {(STATE)MISS, "M"},
    {(STATE)INTACT, "I"},
    {(STATE)INJURED, "X"},
    {(STATE)DESTROYED, "D"},
    {(STATE)UNKNOWN, "U"}};


enum SIGNAL_TOCLIENT {GET_TARGET, RECV_FIELDS, WIN, LOSE, TIE_RES, OPPONENT_MOVE, NO_MORE_SHIPS};
struct cmp_str
{
   bool operator()(char const *a, char const *b) const
   {
      return std::strcmp(a, b) < 0;
   }
};
static std::map <const char*, SIGNAL_TOCLIENT, cmp_str> SIGNAL_TOCLIENT_FROM_STR = {
    {"GET_TARGET", (SIGNAL_TOCLIENT)GET_TARGET},
    {"RECV_FIELDS", (SIGNAL_TOCLIENT)RECV_FIELDS},
    {"WIN", (SIGNAL_TOCLIENT)WIN},
    {"LOSE", (SIGNAL_TOCLIENT)LOSE},
    {"TIE_RES", (SIGNAL_TOCLIENT)TIE_RES},
    {"OPPONENT_MOVE", (SIGNAL_TOCLIENT)OPPONENT_MOVE},
    {"NO_MORE_SHIPS", (SIGNAL_TOCLIENT)NO_MORE_SHIPS}};

enum SIGNAL_TOSERVER {MULTIPLAYER_MODE, BOT_MODE};
static std::map <const char*, SIGNAL_TOSERVER, cmp_str> SIGNAL_TOSERVER_FROM_STR = {
    {"MULTIPLAYER_MODE", (SIGNAL_TOSERVER)MULTIPLAYER_MODE},
    {"BOT_MODE", (SIGNAL_TOSERVER)BOT_MODE}};

enum GAME_RESULT {WON, LOST, TIE, UNFINISHED_GAME};

enum JUDGE_CONCLUSION {INVALID_TARGET, HITTED, MISSED, DESTROYED_ALL};

static const int MAX_CLIENTS = 2;
static const int SOCK_INITIAL_PORT = 8400;
static const char* const SOCK_IP = "127.0.0.1";

#endif