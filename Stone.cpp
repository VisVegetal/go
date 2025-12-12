#include "Stone.hpp"

Stone::Stone(int row, int col, ObjectColor c)
    :BoardObject(row, col, c){}

BoardObject* Stone::clone() const {
    return new Stone(*this);
}

char Stone::getDisplayChar() const {
    if (getColor() == ObjectColor::Black)
        return 'B';
    return 'W';
}

Stone& Stone::operator=(const Stone& other) {
    if (this != &other)
        BoardObject::operator=(other);
    return *this;
}

