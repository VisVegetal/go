#ifndef GUI_HPP
#define GUI_HPP

#include "Interface.hpp"
#include <SFML/Graphics.hpp>

class GUI : public Interface {
public:
    GUI();
    virtual ~GUI() override = default;

    void displayWelcomeMessage() const override;
    void displayBoard(const Board& board) const override;
    void displayError(const CustomGoException& ex) const override;
    void displayMessage(const std::string& message) const override;
    void displayScore(int blackScore, int whiteScore) const override;

    std::pair<int, int> getMoveInput(const std::string& playerName) const override;
    bool askForFortifiedStone(const std::string& playerName, int cost) const override;
};

#endif