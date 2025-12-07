#include "BoardObject.hpp"
#include <algorithm> //std::swap

BoardObject::BoardObject(int row, int col, ObjectColor c)
    : coords(row, col), color(c) {}

BoardObject::BoardObject(const BoardObject& other)
    : coords(other.coords), color(other.color) {}

BoardObject& BoardObject::operator=(const BoardObject& other) {
    BoardObject temp(other);

    using std::swap;
    swap(coords, temp.coords);
    swap(color, temp.color);
    return *this;
}

int BoardObject::getCaptureResistance() const {
    return 1;
}

std::pair<int, int> BoardObject::getCoords() const {
    return coords;
}

ObjectColor BoardObject::getColor() const {
    return color;
}

void BoardObject::setCoords(int row, int col) {
    coords = std::make_pair(row, col);
}




