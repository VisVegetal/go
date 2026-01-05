#include "GoExceptions.hpp"
#include "Joc.hpp"
#include <algorithm>
#include <iostream>

int Joc::numarPartideIncepute;

Joc::Joc(Dimensiuni dim, Jucator* jn, Jucator* ja, float komi)
    : tabla(dim), jucatorNegru(jn), jucatorAlb(ja),
      culoareTurn(Culoare::Negru), reguli (false, true, false, komi, dim, 0) {

    if (jn == nullptr || ja == nullptr)
        throw GoException("Constructor Joc: Jucatorii nu pot fi nuli!");

    numarPartideIncepute++;
}

Joc::Joc(const Joc& other)
    : tabla(other.tabla),
      jucatorNegru(other.jucatorNegru->clone()),
      jucatorAlb(other.jucatorAlb->clone()),
      culoareTurn(other.culoareTurn),
      reguli(other.reguli) {}

Joc::~Joc() {
    delete jucatorAlb;
    delete jucatorNegru;
}

void swap(Joc& first, Joc& second) noexcept {
    using std::swap;
    swap(first.tabla, second.tabla);
    swap(first.jucatorNegru, second.jucatorNegru);
    swap(first.jucatorAlb, second.jucatorAlb);
    swap(first.culoareTurn, second.culoareTurn);
    swap(first.reguli, second.reguli);
}

Joc& Joc::operator=(Joc other) {
    swap(*this, other);
    return *this;
}

void Joc::afiseazaTipJucatori() const {
    std::cout << "Componenta partidei:\n";

    if (dynamic_cast<JucatorBot*>(jucatorNegru))
        std::cout << "- Jucatorul Negru (" << jucatorNegru->getNume() << ") este un Bot.\n";
    else
        std::cout << "- Jucatorul Negru (" << jucatorNegru->getNume() << ") este un Om.\n";


    if (dynamic_cast<JucatorBot*>(jucatorAlb))
        std::cout << "- Jucatorul Alb (" << jucatorAlb->getNume() << ") este un Bot.\n";
    else
        std::cout << "- Jucatorul Alb (" << jucatorAlb->getNume() << ") este un Om.\n";

}

bool Joc::aplicaMutare(const Mutare &m) {
    if (reguli.getSfarsitJoc())
        throw MutareIlegalaException("Jocul s-a terminat deja!");

    if (m.isPass()) {
        std::cout << "Jocul a pasat.\n";
        reguli.incrementPass();
        return true;
    }

    Pozitie p = m.getPozitie();

    if (p.x >= tabla.getMarime() || p.y >= tabla.getMarime())
        throw CoordonateInvalideException(p.x, p.y);

    if (!tabla.esteGol(p))
        throw MutareIlegalaException("Pozitia este deja ocupata.");

    tabla.Plaseazapiatra(p, culoareTurn);
    reguli.resetPass();

    return true;
}

void Joc::joaca() {
    afiseazaTipJucatori();

    while (!reguli.getSfarsitJoc()) {
        try {
            std::cout << "\n" << tabla;
            Jucator* curent = (culoareTurn == Culoare::Negru) ? jucatorNegru : jucatorAlb;

            Mutare m = curent -> alegeMutare(tabla);

            if (aplicaMutare(m))
                culoareTurn = (culoareTurn == Culoare::Negru) ? Culoare::Alb : Culoare::Negru;

        }catch (const GoException& e) {
            std::cout << "\n EROARE JOC: " <<e.what()<< "Incearca din nou.\n";
        }
    }

    std::cout << "\nPartida s-a incheiat!\n";
}