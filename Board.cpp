#include "Board.hpp"
#include "GoExceptions.hpp"
#include <algorithm>

Board::Board() : size(GoRules::getBoardSize()), grid(nullptr) {
    if (size <= 0 || size > 25) {
        throw GameSetupException("Dimensiunea tablei specificata in GoRules este invalida.");
    }
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

void Board::deepCopyGrid(const Board& other) {
    size = other.size;
    initializeGrid();

    for (int i = 0; i < size; ++i) {
        for (int j = 0; j < size; ++j) {
            if (other.grid[i][j] != nullptr) {
                grid[i][j] = other.grid[i][j]->clone();
            }
        }
    }
    // Copierea starii KO (previousState) ar veni aici
}

Board::Board(const Board& other)
    : grid(nullptr), size(0)
{
    deepCopyGrid(other);
}

Board& Board::operator=(const Board& other) {
    Board temp(other);

    using std::swap;
    swap(grid, temp.grid);
    swap(size, temp.size);
    swap(previousState, temp.previousState);

    return *this;
}

void Board::placeStone(int row, int col, ObjectColor color, bool isFortified) {
    if (!isValid(row, col)) {
        throw InvalidInputException("Coordonatele sunt in afara tablei.");
    }
    if (grid[row][col] != nullptr) {
        throw PositionOccupiedException();
    }

    if (isFortified) {
        grid[row][col] = new FortifiedStone(row, col, color);
    } else {
        grid[row][col] = new Stone(row, col, color);
    }
}

void Board::removeStone(int row, int col) {
    if (isValid(row, col) && grid[row][col] != nullptr) {
        delete grid[row][col];
        grid[row][col] = nullptr;
    }
}

void Board::validateMove(int row, int col, ObjectColor color) const {
    if (!isValid(row, col)) {
        throw InvalidInputException("Mutare in afara tablei.");
    }
    if (grid[row][col] != nullptr) {
        throw PositionOccupiedException();
    }
    // Aici s-ar face si verificarea KO (GoRules::isKoViolation)
}

int Board::calculateLiberties(int row, int col) const {
    // Implementare placeholder
    return 4;
}

int Board::checkAndCapture(int row, int col, ObjectColor opponentColor) {
    // Implementare placeholder. Aici s-ar folosi BoardObject::getCaptureResistance()
    return 0;
}

void Board::saveStateForKo() {
    // Logica de salvare (simplificata)
    // previousState.push_back(currentState);
}

int Board::getSize() const {
    return size;
}

BoardObject* Board::getObject(int row, int col) const {
    if (isValid(row, col)) {
        return grid[row][col];
    }
    return nullptr;
}

bool Board::isValid(int row, int col) const {
    return row >= 0 && row < size && col >= 0 && col < size;
}

void Board::findGroupAndLiberties(int row, int col, ObjectColor targetColor,
                               std::vector<std::pair<int, int>>& group,
                               std::vector<std::pair<int, int>>& liberties,
                               std::vector<std::vector<bool>>& visited) const
{
    // Logica recursivă DFS/BFS pentru grupuri și libertăți (omisa)
}