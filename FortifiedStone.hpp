#ifndef FORTIFIED_STONE_HPP
#define FORTIFIED_STONE_HPP

#include "Stone.hpp"

class FortifiedStone : public Stone {
public:
    static const int FORTIFIED_RESISTANCE = 2;

    FortifiedStone(int row, int col, ObjectColor c);

    virtual ~FortifiedStone() = default;

    virtual BoardObject* clone() const override;

    virtual char getDisplayChar() const override;

    virtual int getCaptureResistance() const override;
};

#endif