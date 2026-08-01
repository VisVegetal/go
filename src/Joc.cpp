#include "../include/Joc.hpp"

#include <iomanip>
#include <set>
#include <sstream>

#include "../include/GoExceptions.hpp"
#include "../include/GoLogic.hpp"

namespace {

Culoare inversTurn(Culoare c) {
    return (c == Culoare::Negru) ? Culoare::Alb : Culoare::Negru;
}

} // namespace anonim

// --------------------- Constructie ---------------------

Joc::Joc(Dimensiuni dim, Jucator& n, Jucator& a)
    : tabla(dim), negru(n), alb(a), turn(Culoare::Negru) {
    // Starea initiala intra in istoricul Ko (o mutare nu poate recrea o stare veche).
    reguli.seteazaStareInitiala(tabla);

    istoric.clear();
    istoricIndex = 0;
    salveazaSnapshot();
}

// --------------------- Undo / Redo ---------------------

void Joc::salveazaSnapshot() {
    Snapshot s;
    s.grila = tabla.getGrila();
    s.hashesKo = reguli.getStariKo();
    s.turn = turn;
    s.captN = capturateNegru;
    s.captA = capturateAlb;
    s.jocIncheiat = jocIncheiat;
    s.cedareNegru = cedareNegru;
    s.reguliSfarsit = reguli.getSfarsitJoc();
    s.reguliPass = reguli.getPassConsecutive();

    // Orice mutare noua invalideaza eventualul istoric "redo".
    if (istoricIndex + 1 < istoric.size()) {
        istoric.erase(istoric.begin() + static_cast<long>(istoricIndex + 1), istoric.end());
    }
    istoric.push_back(std::move(s));
    istoricIndex = istoric.size() - 1;
}

void Joc::restaurareDinSnapshot(const Snapshot& s) {
    tabla.setGrila(s.grila);
    reguli.restoreFromSnapshot(s.reguliSfarsit, s.reguliPass, s.hashesKo);
    turn = s.turn;
    capturateNegru = s.captN;
    capturateAlb = s.captA;
    jocIncheiat = s.jocIncheiat;
    cedareNegru = s.cedareNegru;
}

void Joc::undo() {
    if (!poateUndo()) {
        return;
    }
    --istoricIndex;
    restaurareDinSnapshot(istoric[istoricIndex]);
}

void Joc::redo() {
    if (!poateRedo()) {
        return;
    }
    ++istoricIndex;
    restaurareDinSnapshot(istoric[istoricIndex]);
}

// --------------------- Validare & aplicare mutari ---------------------

bool Joc::esteMutareValida(const Mutare& m) const {
    if (jocIncheiat || reguli.getSfarsitJoc()) {
        return false;
    }
    if (m.isPass()) {
        return true;
    }
    return reguli.verificaPlasare(tabla, m.getPozitie(), turn) == Reguli::MotivInvalida::Ok;
}

void Joc::aplicaMutare(const Mutare& m) {
    if (jocIncheiat || reguli.getSfarsitJoc()) {
        throw JocTerminatException();
    }

    // Pass: doua pass-uri consecutive incheie partida.
    if (m.isPass()) {
        reguli.incrementPass();
        turn = inversTurn(turn);
        salveazaSnapshot();
        return;
    }

    const auto p = m.getPozitie();
    switch (reguli.verificaPlasare(tabla, p, turn)) {
        case Reguli::MotivInvalida::Coordonate:
            throw CoordonateInvalideException(p.x, p.y);
        case Reguli::MotivInvalida::Ocupata:
            throw PozitieOcupataException(
                "Pozitia (" + std::to_string(p.x) + "," + std::to_string(p.y) + ") este deja ocupata!");
        case Reguli::MotivInvalida::Suicid:
            throw MutareSuicidalaException(
                "Plasarea in (" + std::to_string(p.x) + "," + std::to_string(p.y) + ") lasa grupul propriu fara libertati!");
        case Reguli::MotivInvalida::Ko:
            throw RegulaKoException(
                "Mutarea ar repeta o stare anterioara a tablei!");
        case Reguli::MotivInvalida::Ok:
            break; // mutare legala
    }

    // Plasarea (cu capturi) a fost deja validata; aplicam fara a mai verifica.
    reguli.resetPass();

    const int capturi = GoLogic::plaseazaCuCapturi(tabla, p, turn);
    if (turn == Culoare::Negru) {
        capturateNegru += capturi;
    } else {
        capturateAlb += capturi;
    }

    reguli.inregistreazaStare(tabla);
    turn = inversTurn(turn);
    salveazaSnapshot();
}

// --------------------- Sugestie mutare ---------------------

bool Joc::sugereazaMutare(Pozitie& out) const {
    if (jocIncheiat || reguli.getSfarsitJoc()) {
        return false;
    }

    const auto n = tabla.getMarime();
    const Pozitie centru{n / 2, n / 2};

    int bestCapt = -1;
    int bestDist = 1'000'000;
    bool gasit = false;

    for (unsigned int x = 0; x < n; ++x) {
        for (unsigned int y = 0; y < n; ++y) {
            const Pozitie p{x, y};
            if (!tabla.esteGol(p)) {
                continue;
            }

            const auto rez = GoLogic::simuleazaPlasare(tabla, p, turn);
            if (!rez.valida || reguli.getStariKo().count(rez.hash) != 0) {
                continue;
            }

            const auto dx = static_cast<int>(p.x) - static_cast<int>(centru.x);
            const auto dy = static_cast<int>(p.y) - static_cast<int>(centru.y);
            const int d = (dx < 0 ? -dx : dx) + (dy < 0 ? -dy : dy);

            if (!gasit || rez.capturi > bestCapt || (rez.capturi == bestCapt && d < bestDist)) {
                bestCapt = rez.capturi;
                bestDist = d;
                out = p;
                gasit = true;
            }
        }
    }
    return gasit;
}

// --------------------- Scor final ---------------------

std::string Joc::determinaCastigator() const {
    const auto [teritoriuNegru, teritoriuAlb] = GoLogic::calculeazaTeritoriu(tabla);

    const float scorNegru = static_cast<float>(capturateNegru + teritoriuNegru);
    const float scorAlb = static_cast<float>(capturateAlb + teritoriuAlb) + komi;

    std::stringstream ss;
    ss << std::fixed << std::setprecision(1);
    ss << "Scor: Negru " << static_cast<int>(scorNegru) << " - Alb " << scorAlb
       << " (Capturi: " << capturateNegru << "/" << capturateAlb
       << ", Teritoriu: " << teritoriuNegru << "/" << teritoriuAlb
       << ", Komi: " << komi << ")";

    // Cedare: cel care a cedat pierde automat, indiferent de scor.
    // Chiar daca adversarul nu l-a intrecut cu komi-ul, cedarea e pierdere.
    if (jocIncheiat) {
        return ss.str() + (cedareNegru ? "\nCastigator: Alb!" : "\nCastigator: Negru!");
    }

    return ss.str() + (scorNegru > scorAlb ? "\nCastigator: Negru!" : "\nCastigator: Alb!");
}
