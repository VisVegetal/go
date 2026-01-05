#ifndef JOC_HPP
#define JOC_HPP

#include <vector>
#include <memory>
#include "Tabla.hpp"
#include "Jucator.hpp"
#include "Reguli.hpp"

class Joc {
private:
    Tabla tabla;
    Jucator* jucatorNegru;
    Jucator* jucatorAlb;
    Culoare culoareTurn;
    Reguli reguli;

    static int numarPartideIncepute;

public:
    Joc(Dimensiuni dim, Jucator* jn, Jucator* ja, float komi);
    Joc(const Joc& other);
    Joc& operator=(Joc other);
    ~Joc();

    friend void swap(Joc& first, Joc& second) noexcept;

    static int getStatisticaJocuri() {return numarPartideIncepute;}

    void joaca();
    bool aplicaMutare(const Mutare& m);

    void afiseazaTipJucatori() const;
};

#endif
