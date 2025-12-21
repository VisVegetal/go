#include "Game.hpp"
#include "HumanPlayer.hpp"
#include "AIPlayer.hpp"
#include "GoRules.hpp"
#include <algorithm>
#include <utility>

Game::Game(Player* p1, Player* p2, Interface* ui_impl)
    : ui(ui_impl),
      board(),
      blackCaptures(0),
      whiteCaptures(0),
      isOver(false)
{
    if (p1->getColor() == StoneColor::Black) {
        blackPlayer = p1;
        whitePlayer = p2;
    } else {
        blackPlayer = p2;
        whitePlayer = p1;
    }
    currentPlayer = blackPlayer;

    int cost = GoRules::getFortifiedStoneCost();
    blackSpecialResource = cost * 2;
    whiteSpecialResource = cost * 2;
}

Game::~Game() {
    delete blackPlayer;
    delete whitePlayer;
    delete ui;
}

void Game::deepCopy(const Game& other) {
    blackPlayer = other.blackPlayer ? other.blackPlayer->clone() : nullptr;
    whitePlayer = other.whitePlayer ? other.whitePlayer->clone() : nullptr;
    ui = nullptr;

    board = other.board;
    blackCaptures = other.blackCaptures;
    whiteCaptures = other.whiteCaptures;
    isOver = other.isOver;
    blackSpecialResource = other.blackSpecialResource;
    whiteSpecialResource = other.whiteSpecialResource;

    if (other.currentPlayer && other.blackPlayer && other.currentPlayer->getColor() == other.blackPlayer->getColor()) {
        currentPlayer = blackPlayer;
    } else {
        currentPlayer = whitePlayer;
    }
}

Game::Game(const Game& other)
    : blackPlayer(nullptr), whitePlayer(nullptr), ui(nullptr)
{
    deepCopy(other);
}

Game& Game::operator=(const Game& other) {
    if (this != &other) {
        Game temp(other);
        std::swap(blackPlayer, temp.blackPlayer);
        std::swap(whitePlayer, temp.whitePlayer);
        std::swap(ui, temp.ui);
        std::swap(board, temp.board);
        std::swap(currentPlayer, temp.currentPlayer);
        std::swap(blackCaptures, temp.blackCaptures);
        std::swap(whiteCaptures, temp.whiteCaptures);
        std::swap(isOver, temp.isOver);
        std::swap(blackSpecialResource, temp.blackSpecialResource);
        std::swap(whiteSpecialResource, temp.whiteSpecialResource);
    }
    return *this;
}

void Game::startGame() {
    ui->displayWelcomeMessage();
    isOver = false;

    while (!isOver) {
        ui->displayBoard(board);
        ui->displayMessage("Randul lui " + currentPlayer->getName());
        processTurn();
        if (!isOver) {
            switchTurn();
        }
    }

    ui->displayMessage("Jocul s-a terminat!");
    ui->displayScore(blackCaptures, whiteCaptures);
}

void Game::processTurn() {
    std::pair<int, int> move_coords = {-3, -3};
    bool move_success = false;

    while (!move_success) {
        try {
            if (dynamic_cast<HumanPlayer*>(currentPlayer)) {
                 move_coords = ui->getMoveInput(currentPlayer->getName());
            } else {
                 move_coords = currentPlayer->getMove();
            }

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

        } catch (const CustomGoException& e) {
            ui->displayError(e);
        }
    }
}

void Game::handleMove(int row, int col) {
    ObjectColor playerColor = (ObjectColor)currentPlayer->getColor();
    ObjectColor opponentColor = (playerColor == ObjectColor::Black)
                                ? ObjectColor::White : ObjectColor::Black;

    board.validateMove(row, col, playerColor);

    bool isFortified = false;
    int cost = GoRules::getFortifiedStoneCost();
    int* currentResource = (playerColor == ObjectColor::Black)
                            ? &blackSpecialResource : &whiteSpecialResource;

    if (*currentResource >= cost) {
        if (ui->askForFortifiedStone(currentPlayer->getName(), *currentResource)) {
            isFortified = true;
            *currentResource -= cost;
        }
    }

    board.placeStone(row, col, playerColor, isFortified);

    int capturedStones = board.checkAndCapture(row, col, opponentColor);

    if (capturedStones == 0 && board.calculateLiberties(row, col) == 0) {
        board.removeStone(row, col);
        throw InvalidMoveException("Sinucidere detectata!");
    }

    if (playerColor == ObjectColor::Black) {
        blackCaptures += capturedStones;
    } else {
        whiteCaptures += capturedStones;
    }

    board.saveStateForKo();
}

void Game::switchTurn() {
    currentPlayer = (currentPlayer == blackPlayer) ? whitePlayer : blackPlayer;
}