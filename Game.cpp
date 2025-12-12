#include "Game.hpp"
#include "GoRules.hpp"

#include <algorithm>
#include <iostream>

#include "HumanPlayer.hpp"

Game::Game(Player* p1, Player* p2, Interface* ui_impl)
    :blackPlayer(p1),
     whitePlayer(p2),
     ui(ui_impl),
     board(),
     blackCaptures(0),
     whiteCaptures(0),
     isOver(false),
     blacSpecialResource(0),
     whiteSpecialResource(0) {

    if (p1->getColor() == StoneColor::Black) {
        blackPlayer = p1;
        whitePlayer = p2;
    }
    else {
        blackPlayer = p2;
        whitePlayer = p1;
    }
    currentPlayer = blackPlayer;

    blacSpecialResource = GoRules::getFortifiedStoneCost() * 2;
    whiteSpecialResource = GoRules::getFortifiedStoneCost() * 2;
}

void Game::cleanup() {
    delete blackPlayer;
    delete whitePlayer;
    delete ui;
}

Game::~Game(){}

void Game::deepCopy(const Game& other) {
    blackPlayer = other.blackPlayer->clone();
    whitePlayer = other.whitePlayer->clone();
    ui = other.ui;

    board = other.board;

    blackCaptures = other.blackCaptures;
    whiteCaptures = other.whiteCaptures;
    isOver = other.isOver;
    blacSpecialResource = other.blacSpecialResource;
    whiteSpecialResource = other.whiteSpecialResource;

    if (other.currentPlayer->getColor() == StoneColor::Black) {
        currentPlayer = blackPlayer;
    }
    else
        currentPlayer = whitePlayer;

}

Game::Game(const Game& other)
    : blackPlayer(nullptr), whitePlayer(nullptr), ui(nullptr) {
    deepCopy(other);
}

Game& Game::operator=(const Game& other) {
    Game temp(other);

    using std::swap;
    swap(blackPlayer, temp.blackPlayer);
    swap(whitePlayer, temp.whitePlayer);
    swap(ui, temp.ui);
    swap(board, temp.board);
    swap(currentPlayer, temp.currentPlayer);
    swap(blackCaptures, temp.blackCaptures);
    swap(whiteCaptures, temp.whiteCaptures);
    swap(isOver, temp.isOver);
    swap(blacSpecialResource, temp.blacSpecialResource);
    swap(whiteSpecialResource, temp.whiteSpecialResource);
    return *this;
}

void Game::startGame() {
    ui->displayWelcomeMessage();
    isOver = false;
    while (!isOver) {
        ui->displayBoard(board);
        processTurn();
        switchTurn();
    }
    ui->displayMessage("Jocul s-a terminat:(");
    ui->displayScore(blackCaptures, whiteCaptures);
}

void Game::processTurn() {
    std::pair<int, int> move_coords = {-3, -3};
    bool move_success = false;

    while (!move_success) {
        try {
            move_coords = currentPlayer->getMove();

            if (dynamic_cast<HumanPlayer*>(currentPlayer))
                move_coords = ui->getMoveInput(currentPlayer->getName());

            if (move_coords.first == -1) {
                ui->displayMessage(currentPlayer->getName() + " a pasat.");
                move_success = true;
                return;
            }
            if (move_coords.first == -2) {
                ui->displayMessage(currentPlayer->getName() + " a abandonat.");
                isOver = true;
                move_success = true;
                return;
            }
            handleMove(move_coords.first, move_coords.second);
            move_success = true;
            catch (const CustomGoException& e) {
                ui->displayError(e);
            }
        }
    }
}

void Game::handleMove(int row, int col) {
    board.validateMove(row, col, currentPlayer->getColor());

    bool isFortifies = false;
    int cost = GoRules::getFortifiedStoneCost();

    if (currentPlayer->getColor() == StoneColor::Black && blacSpecialResource >= cost) {
        if (ui-> askForFortifiedStone(currentPlayer->getName(), blacSpecialResource)) {
            isFortifies = true;
            blacSpecialResource -= cost;
        }
    }
}
