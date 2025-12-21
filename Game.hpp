#ifndef GAME_HPP
#define GAME_HPP

#include "Player.hpp"
#include "Board.hpp"
#include "Interface.hpp"

class Game {
private:
    Player *blackPlayer, *whitePlayer, *currentPlayer;
    Interface* ui;
    Board board;
    int blackCaptures, whiteCaptures;
    bool isOver;

public:
    Game(Player* p1, Player* p2, Interface* ui_impl);
    ~Game();
    Game(const Game& other);
    Game& operator=(const Game& other);

    void startGame();
    void processTurn();

private:
    void handleMove(int row, int col);
    void switchTurn();
};

#endif // GAME_HPP