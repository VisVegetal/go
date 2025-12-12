#include "AIPlayer.hpp"
#include <algorithm>

AIPlayer::AIPlayer(const std::string& n, StoneColor c, int level)
    : Player(n, c), difficultyLevel(level)
{
}

std::pair<int, int> AIPlayer::getMove() const {
    return calculateBestMove();
}

std::pair<int, int> AIPlayer::calculateBestMove() const {
    // Placeholder AI: returneaza o mutare constanta
    return {0, 0};
}

Player* AIPlayer::clone() const {
    return new AIPlayer(*this);
}

AIPlayer::AIPlayer(const AIPlayer& other)
    : Player(other),
      difficultyLevel(other.difficultyLevel)
{
}

AIPlayer& AIPlayer::operator=(const AIPlayer& other) {
    AIPlayer temp(other);

    using std::swap;
    // Swap-ul bazei este gestionat de Player::operator=
    Player::operator=(other);

    swap(difficultyLevel, temp.difficultyLevel);

    return *this;
}

int AIPlayer::getDifficultyLevel() const {
    return difficultyLevel;
}