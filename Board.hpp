#ifndef BOARD_HPP
#define BOARD_HPP

#include "GoRules.hpp"
#include "BoardObject.hpp"
#include "Stone.hpp"
#include "FortifiedStone.hpp"
#include "GoExceptions.hpp"

#include <vector>
#include <utility>

class Board {
private:
    BoardObject*** grid;
    int size;
    std::vector<std::vector<ObjectColor>> previousState;

public:
    Board();
    ~Board();

    // Copy and Swap (Deep Copy)
    Board(const Board& other);
    Board& operator=(const Board& other);

    void placeStone(int row, int col, ObjectColor color, bool isFortified = false);
    void validateMove(int row, int col, ObjectColor color) const;
    int calculateLiberties(int row, int col) const;
    int checkAndCapture(int row, int col, ObjectColor opponentColor);
    void saveStateForKo();
    void removeStone(int row, int col); // Functie suplimentara necesara

    int getSize() const;
    BoardObject* getObject(int row, int col) const;

private:
    void initializeGrid();
    void deepCopyGrid(const Board& other);
    void clearGrid();
    bool isValid(int row, int col) const;

    // Ajuta la recursivitate
    void findGroupAndLiberties(int row, int col, ObjectColor targetColor,
                               std::vector<std::pair<int, int>>& group,
                               std::vector<std::pair<int, int>>& liberties,
                               std::vector<std::vector<bool>>& visited) const;
};

#endif // BOARD_HPP