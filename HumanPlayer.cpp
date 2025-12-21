#include "HumanPlayer.hpp"

HumanPlayer::HumanPlayer(const std::string& n, StoneColor c)
    : Player(n, c) {}

HumanPlayer::HumanPlayer(const HumanPlayer &other)
    : Player(other) {}

HumanPlayer& HumanPlayer::operator=(const HumanPlayer& other) {
    Player::operator=(other);
    return *this;
}

Player* HumanPlayer::clone() const {
    return new HumanPlayer(*this);
}

std::pair<int, int> HumanPlayer::getMove() const {
    return {0,0};
}
