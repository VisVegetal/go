#include "Reguli.hpp"

Reguli::Reguli(bool permiteSuicid_, bool regulaKo_, bool sfarsitJoc_, float komi_, Dimensiuni dim, unsigned int pass_)
    : permiteSuicid(permiteSuicid_),
      regulaKo(regulaKo_),
      sfarsitJoc(sfarsitJoc_),
      komi(komi_),
      dimensiuneTabla(dim),
      passConsecutive(pass_) {}

bool Reguli::getSfarsitJoc() const {
    return sfarsitJoc;
}

float Reguli::getKomi() const {
    return komi;
}

unsigned int Reguli::getPassConsecutive() const {
    return passConsecutive;
}

void Reguli::setSfarsitJoc(bool stare) {
    sfarsitJoc = stare;
}

void Reguli::incrementPass() {
    passConsecutive++;
    if (passConsecutive >= 2) {
        sfarsitJoc = true;
    }
}

void Reguli::resetPass() {
    passConsecutive = 0;
}

bool Reguli::esteMutareValida(const Tabla& tabla, const Mutare& mutare) const {
    if (sfarsitJoc) return false;
    if (mutare.isPass()) return true;

    Pozitie p = mutare.getPozitie();
    return tabla.esteGol(p);
}