#ifndef JOC_HPP
#define JOC_HPP

#include <vector>
#include <string>
#include <optional>   // NEW
#include "Reguli.hpp"
#include "Tabla.hpp"
#include "Jucator.hpp"

//gestionarea unei partide de go
class Joc {
private:
    Tabla tabla;
    Jucator *negru, *alb;
    Culoare turn;//indica cine urmeaza sa mute
    Reguli reguli;

    //matrice pentru salvarea configuratiei precedente, folosita pentru aplicarea regulii KO
    std::vector<std::vector<Culoare>> stareAnterioara;

    //scorul jucatorilor
    int capturateNegru = 0;
    int capturateAlb = 0;

    static int numarPartideIncepute;
    bool jocIncheiat = false;
    float komi = 6.5f; // bonus de puncte pentru jucatorul cu piesele albe

    //undo/redo snapshots
    struct Snapshot {
        std::vector<std::vector<Culoare>> grila;
        std::vector<std::vector<Culoare>> stareKo;
        Culoare turn;
        int captN;
        int captA;
        bool jocIncheiat;
        bool reguliSfarsit;
        unsigned int reguliPass;
    };

    std::vector<Snapshot> istoric;
    std::size_t istoricIndex = 0;

    void salveazaSnapshot(); // NEW

public:
    //configurarea partidei initiale
    Joc(Dimensiuni dim, Jucator* n, Jucator* a);
    ~Joc();
    [[maybe_unused]][[nodiscard]] Culoare getTurnActual() const { return turn; }
    [[maybe_unused]][[nodiscard]] const Tabla& getTabla() const { return tabla; }
    [[maybe_unused]][[nodiscard]] int getCapturateNegru() const { return capturateNegru; }
    [[maybe_unused]][[nodiscard]] int getCapturateAlb() const { return capturateAlb; }

    // Aplicarea mutarilor si verificarea starii jocului
    [[maybe_unused]]void aplicaMutare(const Mutare& m);

    //verifica daca jocul s-a terminat
    [[maybe_unused]][[nodiscard]] bool esteIncheiat() const { return jocIncheiat; }
    void terminaJoc() { jocIncheiat = true; }//oprirea fortata a jocului

    [[nodiscard]] std::string determinaCastigator() const; //scor final (puncte+komi)

    void verificaCapturi(Pozitie p);//verifica daca s-a efectuat o captura prin inconjurarea unei piese

    // undo/redo
    [[nodiscard]] bool poateUndo() const { return istoricIndex > 0; }
    [[nodiscard]] bool poateRedo() const { return istoricIndex + 1 < istoric.size(); }
    [[maybe_unused]]void undo();
    [[maybe_unused]]void redo();

    // hint mutare
    [[nodiscard]] std::optional<Pozitie> sugereazaMutare() const;
};

#endif
