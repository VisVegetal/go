#ifndef POZITIE_HPP
#define POZITIE_HPP

#include <tuple>
#include <iostream>

struct Pozitie {
    unsigned int x, y;
    bool operator<(const Pozitie& other) const {
        return std::tie(x, y) < std::tie(other.x, other.y);
    }

    bool operator==(const Pozitie& other) const {
        return x == other.x && y == other.y;
    }
};

inline std::ostream& operator<<(std::ostream& os, const Pozitie& p) {
    os << "(" << p.x << ", " << p.y << ")";
    return os;
}

#endif