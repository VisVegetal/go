#ifndef GAME_HPP
#define GAME_HPP

#include "Player.hpp"
#include "Board.hpp"
#include "Interface.hpp"
#include "GoExceptions.hpp"

#include <string>
#include <utility>
#include <vector>

class Game {
private:
    Player* blackPlayer;
    Player* whitePlayer;
    Interface* ui;

    Board board;

    Player* currentPlayer;
    int blackCaptures;
    int whiteCaptures;
    bool isOver;
    int blacSpecialResource;
    int whiteSpecialResource;

public:
    Game(Player* p1, Player* p2, Interface* ui_impl);

    ~Game();

    Game(const Game& other);
    Game& operator=(const Game& other);

    void startGame();
    void processTurn();
    void handleMove(int row, int col);
    void handleSpecialMove(Player* player);

private:
    void deepCopy(const Game& other);
    void switchTurn();
    void cleanup();

};
#endif