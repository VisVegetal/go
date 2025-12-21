#ifndef INTERFACE_HPP
#define INTERFACE_HPP

#include "Board.hpp"
#include "GoExceptions.hpp"
#include <utility>
#include <string>

class Interface {
public:
    virtual ~Interface() = default;

    virtual void displayWelcomeMessage() const = 0;
    virtual void displayBoard(const Board& board) const = 0;
    virtual void displayError(const CustomGoException& ex) const = 0; // Observă &
    virtual void displayMessage(const std::string& message) const = 0;
    virtual void displayScore(int blackScore, int whiteScore) const = 0;

    virtual std::pair<int, int> getMoveInput(const std::string& playerName) const = 0;
    virtual bool askForFortifiedStone(const std::string& playerName, int cost) const = 0;
};

#endif