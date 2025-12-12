#ifndef HUMAN_PLAYER_HPP
#define HUMAN_PLAYER_HPP

#include "Player.hpp"

class HumanPlayer : public Player {
public:
    HumanPlayer(const std::string& n, StoneColor c);
    virtual ~HumanPlayer() = default;

    virtual std::pair<int, int> getMove() const override;
    virtual Player* clone() const override;

    HumanPlayer(const HumanPlayer& other);
    HumanPlayer& operator=(const HumanPlayer& other);
};

#endif