#include "../include/Reguli.hpp"
#include "../include/GoLogic.hpp"

void Reguli::incrementPass() {
    ++passConsecutive;
    if (passConsecutive >= 2) {
        sfarsitJoc = true; // doua pass-uri consecutive incheie partida
    }
}

void Reguli::resetPass() {
    passConsecutive = 0;
}

void Reguli::seteazaStareInitiala(const Tabla& t) {
    stariKo.clear();
    stariKo.insert(GoLogic::hashTabla(t));
}

void Reguli::inregistreazaStare(const Tabla& t) {
    stariKo.insert(GoLogic::hashTabla(t));
}

Reguli::MotivInvalida Reguli::verificaPlasare(const Tabla& t, Pozitie p, Culoare c) const {
    const auto n = t.getMarime();

    if (p.x >= n || p.y >= n) {
        return MotivInvalida::Coordonate;
    }
    if (!t.esteGol(p)) {
        return MotivInvalida::Ocupata;
    }

    // Simularea tine cont si de capturile produse, deci detecteaza
    // corect sinuciderea (un grup ar ramane fara libertati chiar si
    // dupa capturarea pietrelor adverse adiacente).
    const auto rez = GoLogic::simuleazaPlasare(t, p, c);
    if (!rez.valida) {
        return MotivInvalida::Suicid;
    }

    // Daca starea rezultata a mai fost atinsa vreodata, mutarea incalca Ko.
    if (stariKo.count(rez.hash) != 0) {
        return MotivInvalida::Ko;
    }

    return MotivInvalida::Ok;
}

void Reguli::restoreFromSnapshot(bool sfarsit, unsigned int passuri,
                                 const std::unordered_set<std::size_t>& hashesKo) {
    sfarsitJoc = sfarsit;
    passConsecutive = passuri;
    stariKo = hashesKo;
}
