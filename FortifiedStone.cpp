#include "FortifiedStone.hpp"

FortifiedStone::FortifiedStone(int row, int col, ObjectColor c)
    : Stone(row, col, c){}

BoardObject* FortifiedStone::clone() const {
    return new FortifiedStone(*this);
}

char FortifiedStone::getDisplayChar() const {
    return 'F';
}

int FortifiedStone::getCaptureResistance() const {
    return FORTIFIED_RESISTANCE;
}


