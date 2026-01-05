#include "Tabla.hpp"
#include <iostream>

Tabla::Tabla(Dimensiuni dim) : dimensiune(dim) {
    unsigned int n = getMarime();
    grila.resize(n, std::vector<Culoare>(n, Culoare::Gol));
}

unsigned int Tabla::getMarime() const {
    if (dimensiune == Dimensiuni::D9x9) return 9;
    if (dimensiune == Dimensiuni::D13x13) return 13;
    return 19;
}

bool Tabla::esteGol(const Pozitie &p) const {
    unsigned int n = getMarime();
    if (p.x < n && p.y < n)
        return grila[p.x][p.y] == Culoare::Gol;
    return false;
}

void Tabla::Plaseazapiatra(const Pozitie& p, Culoare c) {
    unsigned int n = getMarime();
    if (p.x < n && p.y < n)
        grila[p.x][p.y] = c;
}

void Tabla::ScoatePiatra(const Pozitie &p) {
    unsigned int n = getMarime();
    if (p.x < n && p.y < n)
        grila[p.x][p.y] = Culoare::Gol;
}

Culoare Tabla::getPozitieCuloare(const Pozitie& p) const {
    unsigned int n = getMarime();
    if (p.x < n && p.y < n)
        return grila[p.x][p.y];
    return Culoare::Gol;
}

std::ostream& operator<<(std::ostream& os, const Tabla& t) {
    unsigned int n = t.getMarime();
    os << "   ";
    for (unsigned int i = 0; i < n; ++i)
        os << i % 10 << " ";
    os << std::endl;

    for (unsigned int i = 0; i < n; ++i) {
        os << i % 10 << " ";
        for (unsigned int j = 0; j < n; ++j) {
            switch (t.grila[i][j]) {
                case Culoare::Negru: os << "N "; break;
                case Culoare::Alb:   os << "A "; break;
                case Culoare::Gol:   os << ". "; break;
            }
        }
        os << "\n";
    }
    return os;
}