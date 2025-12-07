#include "Player.hpp"
#include <algorithm> //swap

Player::Player(const std::string& n, StoneColor c)
    : name(n), color(c) {}

Player::Player(const Player& other)
    : name(other.name), color(other.color) {}

Player& Player::operator=(const Player& other) {
    Player temp(other);

    using std::swap;
    swap(name, temp.name);
    swap(color, temp.color);
    return *this;
}

std::string Player::getName() const {
    return name;
}

StoneColor Player::getColor() const {
    return color;
}