#ifndef PLAYER_HPP
#define PLAYER_HPP

#include <string>
#include <utility> //std::pair

enum class StoneColor {
    Black,
    White,
    Empty
};

class Player {
protected:
    std::string name;
    StoneColor color;

public:
    Player(const std::string& n, StoneColor c);

    virtual ~Player() = default;

    Player(const Player& other);
    Player& operator=(const Player& other);

    virtual std::pair<int, int> getMove() const = 0;
    virtual Player* clone() const = 0;

    std::string getName() const;
    StoneColor getColor() const;
};

#endif

