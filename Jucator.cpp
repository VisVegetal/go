#include "Jucator.hpp"
#include "Joc.hpp"
#include <iostream>

Jucator::Jucator(std::string nume_, Culoare culoare_)
    : nume(std::move(nume_)), culoare(culoare_), pietreCapturate(0) {}

Mutare JucatorUman::alegeMutare(const Tabla &t) {
    std::string tip;
    std::cout << nume << " (" << (culoare == Culoare::Negru ? "Negru" : "Alb") << "), introdu mutarea (PLASEAZA x y / PASS): ";
    std::cin >> tip;

    if (tip == "PASS")
        return Mutare({0, 0}, tipM::pass);
    else {
        unsigned int x, y;
        std::cin >> x >> y;
        return Mutare({x, y}, tipM::plasare);
    }

}

Mutare JucatorBot::alegeMutare(const Tabla &t) {
    std::cout << "Bot-ul " << nume << "se gandeste...\n";

    unsigned int n = 9;
    for (unsigned int i = 0; i < n; ++i) {
        for (unsigned int j = 0; j < n; ++j) {
            if (t.esteGol({i, j}))
                return Mutare({i, j}, tipM::plasare);
        }
    }
    return Mutare({0, 0}, tipM:pass);
}


