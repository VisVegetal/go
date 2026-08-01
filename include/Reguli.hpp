#ifndef REGULI_HPP
#define REGULI_HPP

#include <cstddef>
#include <unordered_set>

#include "Mutare.hpp"
#include "Tabla.hpp"

// Gestioneaza aspectele de stare ale regulilor de Go: contorizarea
// pass-urilor consecutive (sfarsitul jocului) si istoricul starilor
// tablei folosit pentru regula Ko. Verificarea de nivel tabla
// (libertati, capturi, sinucidere, hash) este delegata catre GoLogic.
class Reguli {
private:
    bool sfarsitJoc = false;
    unsigned int passConsecutive = 0;

    // Toate starile tablei atinse pana in prezent (hash-uri FNV-1a).
    // Include starea curenta, astfel incat orice mutare care ar duce
    // tabla intr-o stare deja atinsa este respinsa (Ko simplu + superko).
    std::unordered_set<std::size_t> stariKo;

public:
    Reguli() = default;

    [[nodiscard]] bool getSfarsitJoc() const { return sfarsitJoc; }

    void incrementPass();
    void resetPass();

    [[nodiscard]] unsigned int getPassConsecutive() const { return passConsecutive; }

    // --------------------- Regula Ko ---------------------

    // Inregistreaza starea initiala a tablei in istoricul Ko.
    void seteazaStareInitiala(const Tabla& t);

    // Adauga hash-ul starii curente a tablei in istoricul Ko.
    void inregistreazaStare(const Tabla& t);

    [[nodiscard]] const std::unordered_set<std::size_t>& getStariKo() const { return stariKo; }

    // --------------------- Validare mutari ---------------------

    // Motivul pentru care o plasare este (in)valida.
    enum class MotivInvalida { Ok, Coordonate, Ocupata, Suicid, Ko };

    // Verifica complet o plasare pe tabla data: coordonate, ocupatie,
    // interdictia sinuciderii (dupa capturi) si regula Ko.
    [[nodiscard]] MotivInvalida verificaPlasare(const Tabla& t, Pozitie p, Culoare c) const;

    // --------------------- Snapshot pentru undo/redo ---------------------

    void restoreFromSnapshot(bool sfarsit, unsigned int passuri,
                             const std::unordered_set<std::size_t>& hashesKo);
};

#endif
