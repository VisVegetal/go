#ifndef JOC_HPP
#define JOC_HPP

#include "Tabla.hpp"
#include "Jucator.hpp"

class Joc {
private:
    Tabla tabla;
    Jucator *negru, *alb;
    Culoare turn;
    static int numarPartideIncepute;
public:
    Joc(Dimensiuni dim, Jucator* n, Jucator* a);
    Joc(const Joc& other);
    Joc& operator=(Joc other);
    ~Joc();
    friend void swap(Joc& first, Joc& second) noexcept;
    static int getStatisticaJocuri();
    void joaca();
    bool aplicaMutare(const Mutare& m);
};

#endif