#include "Reguli.hpp"

Reguli::Reguli() : sfarsitJoc(false), passConsecutive(0) {}

bool Reguli::getSfarsitJoc() const { return sfarsitJoc; }

//gestiune pass-uri
void Reguli::incrementPass() {
    passConsecutive++;
    if (passConsecutive >= 2) sfarsitJoc = true;
}

//resetare contor pass-uri
void Reguli::resetPass() { passConsecutive = 0; }

bool Reguli::esteMutareValida(const Tabla& t, const Mutare& m) const {
    if (sfarsitJoc) return false; //verifica daca s-a terminat jocul
    if (m.isPass()) return true; //verifica pass
    return t.esteGol(m.getPozitie());//verifica suprapunerea pieselor
}