#include "Tabla.hpp"

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
    return (p.x < n && p.y < n && grila[p.x][p.y] == Culoare::Gol);
}

void Tabla::Plaseazapiatra(const Pozitie& p, Culoare c) {
    grila[p.x][p.y] = c;
}


Culoare Tabla::getPozitieCuloare(const Pozitie& p) const {
    return grila[p.x][p.y];
}

std::ostream& operator<<(std::ostream& os, const Tabla& t) {
    unsigned int n = t.getMarime();
    for (unsigned int i = 0; i < n; ++i) {
        for (unsigned int j = 0; j < n; ++j) {
            if (t.grila[i][j] == Culoare::Gol) os << ". ";
            else os << (t.grila[i][j] == Culoare::Negru ? "N " : "A ");
        }
        os << "\n";
    }
    return os;
}