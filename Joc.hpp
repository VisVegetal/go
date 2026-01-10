#ifndef JOC_HPP
#define JOC_HPP

#include <vector>
#include <string>
#include "Tabla.hpp"
#include "Jucator.hpp"

//gestionarea unei partide de go
class Joc {
private:
    Tabla tabla;
    Jucator *negru, *alb;
    Culoare turn;//indica cine urmeaza sa mute

    //matrice pentru salvarea configuratiei precedente, folosita pentru aplicarea regulii KO
    std::vector<std::vector<Culoare>> stareAnterioara;

    //scorul jucatorilor
    int capturateNegru = 0;
    int capturateAlb = 0;

    static int numarPartideIncepute;
    bool jocIncheiat = false;
    float komi = 6.5f; // bonus de puncte pentru jucatorul cu piesele albe

public:
    //configurarea partidei initiale
    Joc(Dimensiuni dim, Jucator* n, Jucator* a);
    ~Joc();

    // Aplicarea mutarilor si verificarea starii jocului
    void aplicaMutare(const Mutare& m);

    //verifica daca jocul s-a terminat
    [[nodiscard]] bool esteIncheiat() const { return jocIncheiat; }
    void terminaJoc() { jocIncheiat = true; }//oprirea fortata a jocului

    //
    [[nodiscard]] int getCapturateNegru() const { return capturateNegru; }
    [[nodiscard]] int getCapturateAlb() const { return capturateAlb; }
    [[nodiscard]] const Tabla& getTabla() const { return tabla; } // returneaza o referinta constanta catre tabla pentru randare eficienta
    [[nodiscard]] Culoare getTurnActual() const { return turn; }
    [[nodiscard]] std::string determinaCastigator() const; //scor final (puncte+komi)

    void verificaCapturi(Pozitie p);//verifica daca s-a efectuat o captura prin inconjurarea unei piese
};

#endif