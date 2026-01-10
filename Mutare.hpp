#ifndef MUTARE_HPP
#define MUTARE_HPP

#include "Pozitie.hpp"
#include "Culoare.hpp"


class Mutare {
private:
    Pozitie pozitie; // coordonate (x, y)
    tipM tip;

public:
    //initializare mutare
    Mutare(Pozitie p, tipM t) : pozitie (p), tip(t) {}
    //verifica daca e pass
    [[nodiscard]]bool isPass() const {
        return tip == tipM::pass;
    }
    //returneaza coordonatele
    [[nodiscard]]Pozitie getPozitie() const {
        return pozitie;
    }
};
#endif