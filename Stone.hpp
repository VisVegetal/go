#ifndef STONE_HPP
#define STONE_HPP

#include "BoardObject.hpp"

class Stone : public BoardObject {
private:
public:
    Stone(int row, int col, ObjectColor c);

    virtual ~Stone() = default;

    virtual BoardObject* clone() const override;

    virtual char getDisplayChar() const override;

    Stone(const Stone& other);
    Stone& operator=(const Stone& other);
};
#endif