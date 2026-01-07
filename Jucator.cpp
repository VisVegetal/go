#include "Jucator.hpp"
#include <iostream>

Mutare JucatorUman::alegeMutare(const Tabla&) {
    std::string op;
    std::cout << nume << ", alege (P x y / PASS): ";
    std::cin >> op;
    if (op == "PASS") return Mutare({0, 0}, tipM::pass);
    unsigned int x, y;
    std::cin >> x >> y;
    return Mutare({x, y}, tipM::plasare);
}

Mutare JucatorBot::alegeMutare(const Tabla& t) {
    unsigned int n = t.getMarime();
    for(unsigned int i=0; i<n; ++i)
        for(unsigned int j=0; j<n; ++j)
            if(t.esteGol({i, j})) return Mutare({i, j}, tipM::plasare);
    return Mutare({0,0}, tipM::pass);
}