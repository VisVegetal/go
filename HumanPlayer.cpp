#include "HumanPlayer.hpp"

HumanPlayer::HumanPlayer(const std::string& n, StoneColor c)
    : Player(n, c)
{
}

std::pair<int, int> HumanPlayer::getMove() const {
    // Placeholder: Logica de citire input va fi delegată Interfeței în clasa Game
    return {-3, -3};
}

Player* HumanPlayer::clone() const {
    return new HumanPlayer(*this);
}

HumanPlayer::HumanPlayer(const HumanPlayer& other)
    : Player(other)
{
}

HumanPlayer& HumanPlayer::operator=(const HumanPlayer& other) {
    if (this != &other) {
        Player::operator=(other);
    }
    return *this;
}