#ifndef REGULI_HPP
#define REGULI_HPP

#include "Culoare.hpp"
#include "Mutare.hpp"
#include "Tabla.hpp"

class Reguli {
private:
    bool permiteSuicid;
    bool regulaKo;
    bool sfarsitJoc;
    float komi;
    Dimensiuni dimensiuneTabla;
    unsigned int passConsecutive;

public:
    Reguli(bool permiteSuicid_, bool regulaKo_, bool sfarsitJoc_, float komi_, Dimensiuni dim, unsigned int pass_);

    bool getSfarsitJoc() const;
    float getKomi() const;
    unsigned int getPassConsecutive() const;

    void setSfarsitJoc(bool stare);
    void incrementPass();
    void resetPass();

    bool esteMutareValida(const Tabla& tabla, const Mutare& mutare) const;
};

#endif