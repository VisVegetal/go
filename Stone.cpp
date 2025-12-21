#include "Stone.hpp"

Stone::Stone(int row, int col, ObjectColor c) : BoardObject(row, col, c) {}

Stone::Stone(const Stone& other) : BoardObject(other) {}

Stone& Stone::operator=(const Stone& other) {
    if (this != &other) {
        BoardObject::operator=(other);
    }
    return *this;
}

BoardObject* Stone::clone() const {
    return new Stone(*this);
}

char Stone::getDisplayChar() const {
    return (getColor() == ObjectColor::Black) ? 'B' : 'W';
}