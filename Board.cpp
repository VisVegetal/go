#include "Board.hpp"
#include "GoExceptions.hpp"
#include "Stone.hpp"
#include "FortifiedStone.hpp"
#include <algorithm>

Board::Board()
    : size(19), grid(nullptr) {
    initializeGrid();
}

void Board::initializeGrid() {
    grid = new BoardObject**[size];
    for (int i = 0; i < size; ++i) {
        grid[i] = new BoardObject*[size];
        for (int j = 0; j < size; ++j) {
            grid[i][j] = nullptr;
        }
    }
}

void Board::clearGrid() {
    if (grid) {
        for (int i = 0; i < size; ++i) {
            if (grid[i]) {
                for (int j = 0; j < size; ++j) {
                    delete grid[i][j];
                }
                delete[] grid[i];
            }
        }
        delete[] grid;
        grid = nullptr;
    }
}

Board::~Board() {
    clearGrid();
}

Board::Board(const Board &other) : size(other.size), grid(nullptr) {
    initializeGrid();
    for (int i = 0; i < size; ++i) {
        for (int j = 0; j < size; ++j) {
            if (other.grid[i][j])
                grid[i][j] = other.grid[i][j]->clone();

        }
    }
    previousState = other.previousState;
}

Board& Board::operator=(const Board& other) {
    if (this != &other) {
        Board temp(other);
        std::swap(grid, temp.grid);
        std::swap(size, temp.size);
        std::swap(previousState, temp.previousState);
    }
    return *this;
}

void Board::placeStone(int row, int col, ObjectColor color, bool isFortified) {
    if (!isValid(row, col))
        throw InvalidInputException("Coordonate invalide!");
    if (grid[row][col])
        throw PositionOccupiedException();

    if (!isFortified)
        grid[row][col] = new FortifiedStone(row, col, color);
    else
        grid[row][col] = new Stone(row, col, color);

}

void Board::removeStone(int row, int col) {
    if (!isValid(row, col)) {
        delete grid[row][col];
        grid[row][col] = nullptr;
    }
}

bool Board::isValid(int row, int col) const {
    return row >= 0 && row < size && col >= 0 && col < size;
}

int Board::getSize() const {
    return size;
}

BoardObject *Board::getObject(int row, int col) const {
    return isValid(row, col) ? grid[row][col] : nullptr;
}


