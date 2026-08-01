#ifndef GOLOGIC_HPP
#define GOLOGIC_HPP

#include <set>
#include <stack>
#include <vector>
#include <cstddef>
#include <utility>
#include <unordered_set>

#include "Pozitie.hpp"
#include "Culoare.hpp"

class Tabla;

// Motorul de reguli de nivel tabla: grupuri, libertati, capturi,
// interdictia sinuciderii, hashing pentru regula Ko si generarea
// mutarilor valide folosite de boti.
namespace GoLogic {

    constexpr int dx[4] = {-1, 1, 0, 0};
    constexpr int dy[4] = {0, 0, -1, 1};

    // --------------------- Grupuri & libertati ---------------------

    // Umple 'grup' cu toate pozitiile conectate de culoarea 'c' ce pornesc din 'p'.
    void gasesteGrup(const Tabla& t, Pozitie p, Culoare c, std::set<Pozitie>& grup);

    // Returneaza toate grupurile de culoarea 'c' de pe tabla.
    std::vector<std::set<Pozitie>> gasesteToateGrupele(const Tabla& t, Culoare c);

    // Un grup are macar o libertate (un punct gol adiacent)?
    bool areLibertati(const Tabla& t, const std::set<Pozitie>& grup);

    // Numarul de libertati (puncte goale adiacente distincte) ale unui grup.
    unsigned int numarLibertati(const Tabla& t, const std::set<Pozitie>& grup);

    // --------------------- Plasare & capturi ---------------------

    // Plaseaza o piatra si elimina grupurile adverse ramase fara libertati.
    // Returneaza numarul de piese adverse capturate.
    int plaseazaCuCapturi(Tabla& t, Pozitie p, Culoare c);

    // Verifica interdictia sinuciderii: plasarea ar lasa propriul grup fara
    // libertati, chiar si dupa capturarea pieselor adverse adiacente.
    bool esteSuicidala(const Tabla& t, Pozitie p, Culoare c);

    // Rezultatul simularii unei plasari pe o copie a tablei.
    struct RezultatSimulare {
        bool valida;          // plasarea este permisa (nu este suicidala)
        int capturi;          // numarul de piese adverse capturate
        std::size_t hash;     // hash-ul tablei rezultate (pentru regula Ko)
    };

    // Simuleaza plasarea pe o copie a tablei (fara a modifica originalul).
    RezultatSimulare simuleazaPlasare(const Tabla& t, Pozitie p, Culoare c);

    // --------------------- Hashing / Ko ---------------------

    // Hash determinist al configuratiei curente a tablei (FNV-1a).
    std::size_t hashTabla(const Tabla& t);

    // --------------------- Generare mutari pentru boti ---------------------

    // Toate pozitiile goale unde plasarea este legala (nu e suicid si nu incalca Ko).
    std::vector<Pozitie> enumeraMutariValide(const Tabla& t, Culoare c,
                                             const std::unordered_set<std::size_t>& koSet);

    // Varianta detaliata: include numarul de capturi si hash-ul starii rezultate.
    struct MutareCandidat {
        Pozitie poz;
        int capturi;
        std::size_t hashDupa;
    };
    std::vector<MutareCandidat> enumeraCandidati(const Tabla& t, Culoare c,
                                                 const std::unordered_set<std::size_t>& koSet);

    // --------------------- Scor / teritoriu ---------------------

    // Calculeaza teritoriul (regiuni de spatii libere) pentru fiecare jucator:
    // returneaza { teritoriuNegru, teritoriuAlb } (algoritm Tromp-Taylor simplificat).
    std::pair<int, int> calculeazaTeritoriu(const Tabla& t);

}

#endif
