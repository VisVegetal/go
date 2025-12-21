#include "AIPlayer.hpp"

AIPlayer::AIPlayer(const std::string& n, StoneColor c, int level)
    : Player(n, c), difficultyLevel(level)
{}

AIPlayer::AIPlayer(const AIPlayer &other)
    : Player(other), difficultyLevel(other.difficultyLevel){}

AIPlayer& AIPlayer::operator=(const AIPlayer& other) {
    if (this != &other) {
        Player::operator=(other);
        difficultyLevel = other.difficultyLevel;
    }
    return *this;
}

Player* AIPlayer::clone() const {
    return new AIPlayer(*this);
}

int AIPlayer::getDifficultyLevel() const { return difficultyLevel; }

std::pair<int, int> AIPlayer::getMove() const {
    return calculateBestMove();
}

std::pair<int, int> AIPlayer::calculateBestMove() const {
    return {0,0};
}

