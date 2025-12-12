#ifndef AI_PLAYER_HPP
#define AI_PLAYER_HPP

#include "Player.hpp"

class AIPlayer : public Player {
private:
    int difficultyLevel;

public:
    AIPlayer(const std::string& n, StoneColor c, int level);
    virtual ~AIPlayer() = default;

    virtual std::pair<int, int> getMove() const override;
    virtual Player* clone() const override;

    // Copy and Swap
    AIPlayer(const AIPlayer& other);
    AIPlayer& operator=(const AIPlayer& other);

    int getDifficultyLevel() const;

private:
    std::pair<int, int> calculateBestMove() const;
};

#endif