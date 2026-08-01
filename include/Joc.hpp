#ifndef JOC_HPP
#define JOC_HPP

#include <cstddef>
#include <string>
#include <unordered_set>
#include <vector>

#include "Culoare.hpp"
#include "Jucator.hpp"
#include "Mutare.hpp"
#include "Reguli.hpp"
#include "Tabla.hpp"

// Gestionarea unei partide de Go: tabla, jucatori, turn, scoruri,
// regula Ko, undo/redo si calculul castigatorului.
class Joc {
private:
    Tabla tabla;
    Jucator& negru;   // referinte; proprietarul jucatorilor este apelantul (main)
    Jucator& alb;
    Culoare turn;     // cine urmeaza sa mute
    Reguli reguli;    // pass-uri consecutive, sfarsit joc, stare Ko

    int capturateNegru = 0;
    int capturateAlb = 0;

    bool jocIncheiat = false;    // cedare
    bool cedareNegru = false;    // Negrul este cel care a cedat
    float komi = 6.5f;           // bonus pentru Alb

    // Snapshots pentru undo/redo (RAII: toate detinute prin valoare).
    struct Snapshot {
        std::vector<std::vector<Culoare>> grila;
        std::unordered_set<std::size_t> hashesKo;
        Culoare turn;
        int captN;
        int captA;
        bool jocIncheiat;
        bool cedareNegru;
        bool reguliSfarsit;
        unsigned int reguliPass;
    };

    std::vector<Snapshot> istoric;
    std::size_t istoricIndex = 0;

    void salveazaSnapshot();
    void restaurareDinSnapshot(const Snapshot& s);

public:
    Joc(Dimensiuni dim, Jucator& n, Jucator& a);

    // --------------------- Accesori (const-correctness) ---------------------

    [[nodiscard]] Culoare getTurnActual() const { return turn; }
    [[nodiscard]] const Tabla& getTabla() const { return tabla; }
    [[nodiscard]] int getCapturateNegru() const { return capturateNegru; }
    [[nodiscard]] int getCapturateAlb() const { return capturateAlb; }
    [[nodiscard]] const std::unordered_set<std::size_t>& getStariKo() const { return reguli.getStariKo(); }
    [[nodiscard]] float getKomi() const { return komi; }

    // --------------------- Mutari ---------------------

    // Verifica exhaustiv daca mutarea este legala (coordonate, ocupatie,
    // sinucidere, Ko, sfarsitul jocului).
    [[nodiscard]] bool esteMutareValida(const Mutare& m) const;

    // Aplica o mutare; arunca tipuri specifice de GoException la mutari ilegale.
    void aplicaMutare(const Mutare& m);

    // --------------------- Stare joc ---------------------

    [[nodiscard]] bool esteIncheiat() const { return jocIncheiat || reguli.getSfarsitJoc(); }

    // Cedare: cine cedeaza pierde automat, indiferent de scor (nici komi-ul
    // adversarului nu il poate salva pe cel care a cedat).
    void terminaJoc(bool negruCedeaza) {
        jocIncheiat = true;
        cedareNegru = negruCedeaza;
    }

    [[nodiscard]] std::string determinaCastigator() const; // scor final (teritoriu + capturi + komi)

    // --------------------- Undo / Redo ---------------------

    [[nodiscard]] bool poateUndo() const { return istoricIndex > 0; }
    [[nodiscard]] bool poateRedo() const { return istoricIndex + 1 < istoric.size(); }
    void undo();
    void redo();

    // Hint: o mutare buna pentru jucatorul curent (folosita in interfata).
    [[nodiscard]] bool sugereazaMutare(Pozitie& out) const;
};

#endif
