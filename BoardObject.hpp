#ifndef BOARD_OBJECT_HPP
#define BOARD_OBJECT_HPP

#include <utility> // std::pair
#include <string>

enum class ObjectColor {
    Black,
    White,
    Empty
};

class BoardObject {
private:
    std::pair<int, int> coords;
    ObjectColor color;
public:
    BoardObject(int row, int col, ObjectColor c);

    virtual ~BoardObject() = default;

    virtual BoardObject* clone() const = 0;

    virtual char getDisplayChar() const = 0;

    virtual int getCaptureResistance() const;

    BoardObject(const BoardObject&);
    BoardObject& operator=(const BoardObject& other);

    std::pair<int, int> getCoords() const;
    ObjectColor getColor() const;

protected:
    void setCoords(int row, int col);
};

#endif