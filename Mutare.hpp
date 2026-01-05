#ifndef MUTARE_HPP
#define MUTARE_HPP

#include "Pozitie.hpp"
#include <iostream>

enum class tipM {plasasre, pass};

class Mutare {
private:
    Pozitie pozitie;
    tipM tip_m;

public:
    Mutare(Pozitie p, tipM t) : pozitie (p), tip_m(t) {}

    bool isPass() const {return tip_m == tipM::pass;}
    Pozitie getPozitie() const {return pozitie;}
    tipM getTip() const {return tip_m;}

    friend std::ostream& operator<<(std::ostream& os, const Mutare& m) {
        if (m.tip_m == tipM::pass)
            os << "PASS";
        else
            os << "PLASEAZA la " << m.pozitie;
        return os;

    }
};
#endif