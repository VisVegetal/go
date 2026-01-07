#include "Joc.hpp"
#include "GoExceptions.hpp"
#include <algorithm>

int Joc::numarPartideIncepute = 0;

Joc::Joc(Dimensiuni dim, Jucator* n, Jucator* a)
    : tabla(dim), negru(n), alb(a), turn(Culoare::Negru) {
    if(!n || !a) throw GoException("Jucatori invalidi!");
    numarPartideIncepute++;
}

Joc::~Joc() { delete negru; delete alb; }

Joc::Joc(const Joc& other)
    : tabla(other.tabla), negru(other.negru->clone()), alb(other.alb->clone()), turn(other.turn) {}

void swap(Joc& first, Joc& second) noexcept {
    using std::swap;
    swap(first.tabla, second.tabla);
    swap(first.negru, second.negru);
    swap(first.alb, second.alb);
    swap(first.turn, second.turn);
}

Joc& Joc::operator=(Joc other) {
    swap(*this, other);
    return *this;
}

int Joc::getStatisticaJocuri() { return numarPartideIncepute; }

bool Joc::aplicaMutare(const Mutare& m) {
    if (m.isPass()) {
        return true;
    }

    Pozitie p = m.getPozitie();
    unsigned int marime = tabla.getMarime();

    if (p.x >= marime || p.y >= marime) {
        throw CoordonateInvalideException(static_cast<int>(p.x), static_cast<int>(p.y));
    }

    if (!tabla.esteGol(p)) {
        throw MutareIlegalaException("Loc ocupat!");
    }

    tabla.Plaseazapiatra(p, turn);

    return (tabla.getPozitieCuloare(p) == turn);
}

void Joc::joaca() {
    for(size_t i = 0; i < 5; ++i) {
        try {
            std::cout << tabla;
            Jucator* curent = (turn == Culoare::Negru) ? negru : alb;
            if (aplicaMutare(curent->alegeMutare(tabla))) {
                turn = (turn == Culoare::Negru) ? Culoare::Alb : Culoare::Negru;
            }
        } catch (const GoException& e) {
            std::cout << e.what() << " Reincearca!\n";
        }
    }
}