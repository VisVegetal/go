#ifndef BOARD_HPP
#define BOARD_HPP

#include "BoardObject.hpp"
#include <vector>

class Board {
private:
    BoardObject*** grid;
    int size;
    std::vector<std::vector<ObjectColor>> previousState;

public:
    Board();
    ~Board();
    Board(const Board& other);
    Board& operator=(const Board& other);

    void placeStone(int row, int col, ObjectColor color, bool isFortified = false);
    void validateMove(int row, int col, ObjectColor color) const;
    int calculateLiberties(int row, int col) const;
    int checkAndCapture(int row, int col, ObjectColor opponentColor);
    void saveStateForKo();
    void removeStone(int row, int col);

    int getSize() const;
    BoardObject* getObject(int row, int col) const;

private:
    void initializeGrid();
    void clearGrid();
    bool isValid(int row, int col) const;

};

#endif // BOARD_HPP