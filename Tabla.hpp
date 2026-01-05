#ifndef TABLA_HPP
#define TABLA_HPP

#include "Culoare.hpp"
#include "Pozitie.hpp"
#include <vector>
#include <ostream>

class Tabla {
private:
    Dimensiuni dimensiune;
    std::vector<std::vector<Culoare>> grila;

public:
    explicit Tabla(Dimensiuni dim);

    void Plaseazapiatra(const Pozitie& p, Culoare c);
    void ScoatePiatra(const Pozitie& p);
    bool esteGol(const Pozitie& p) const;

    unsigned int getMarime() const;
    Culoare getPozitieCuloare(const Pozitie& p) const;

    friend std::ostream& operator<<(std::ostream& os, const Tabla& t);
};

#endif