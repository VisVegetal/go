#ifndef REGULI_HPP
#define REGULI_HPP

#include "Mutare.hpp"
#include "Tabla.hpp"

class Reguli {
private:
    bool sfarsitJoc;
    unsigned int passConsecutive; // contorizarea pass-urilor (la 2 pass-uri consecutive se opreste jocul)
public:
    Reguli();
    [[nodiscard]]bool getSfarsitJoc() const;
    void incrementPass();
    void resetPass();
    [[nodiscard]] bool esteMutareValida(const Tabla& t, const Mutare& m) const;
};

#endif