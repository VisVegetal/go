#ifndef MUTARE_HPP
#define MUTARE_HPP

#include "Pozitie.hpp"
#include "Culoare.hpp"


class Mutare {
private:
    Pozitie pozitie;
    tipM tip;

public:
    Mutare(Pozitie p, tipM t) : pozitie (p), tip(t) {}
    [[nodiscard]]bool isPass() const {
        return tip == tipM::pass;
    }
    [[nodiscard]]Pozitie getPozitie() const {
        return pozitie;
    }
};
#endif