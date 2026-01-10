#include "Joc.hpp"
#include "GoExceptions.hpp"
#include <set>
#include <iomanip>
#include <sstream>

//initializarea membrului static
int Joc::numarPartideIncepute = 0;

//identifica grupurile de piese conectate folosind algoritmul Flood Fill.
// NOLINTNEXTLINE(misc-no-recursion)
static void gasesteGrup(const Tabla& t, Pozitie p, Culoare c, std::set<Pozitie>& grup) {
    const auto n = t.getMarime(); // Utilizam auto pentru a evita duplicarea tipului
    //conditii de oprire: in afara tablei, culoare diferita sau pozitie deja vizitata
    if (p.x >= n || p.y >= n || t.getPozitieCuloare(p) != c || grup.contains(p)) {
        return;
    }

    grup.insert(p); //adaugam pozitia curenta in grup
    //se verifica cei 4 vecini
    for (int i = 0; i < 4; ++i) {
        constexpr int dx[] = {-1, 1, 0, 0};
        constexpr int dy[] = {0, 0, -1, 1};

        const auto nx = static_cast<int>(p.x) + dx[i];
        const auto ny = static_cast<int>(p.y) + dy[i];

        if (nx >= 0 && nx < static_cast<int>(n) && ny >= 0 && ny < static_cast<int>(n)) {
            gasesteGrup(t, {static_cast<unsigned int>(nx), static_cast<unsigned int>(ny)}, c, grup);
        }
    }
}

//verifica daca un grup mai are libertati, daca returneaza 'false', grupul este capturat
static bool areLibertati(const Tabla& t, const std::set<Pozitie>& grup) {
    const auto n = t.getMarime();

    for (const auto& p : grup) {
        for (int i = 0; i < 4; ++i) {
            constexpr int dx[] = {-1, 1, 0, 0};
            constexpr int dy[] = {0, 0, -1, 1};

            const auto nx = static_cast<int>(p.x) + dx[i];
            const auto ny = static_cast<int>(p.y) + dy[i];
            //// Daca gasim un singur punct gol adiacent, grupul este viu
            if (nx >= 0 && nx < static_cast<int>(n) && ny >= 0 && ny < static_cast<int>(n)) {
                if (t.esteGol({static_cast<unsigned int>(nx), static_cast<unsigned int>(ny)})) {
                    return true;
                }
            }
        }
    }
    return false;
}

//initializare componente joc
Joc::Joc(Dimensiuni dim, Jucator* n, Jucator* a)
    : tabla(dim), negru(n), alb(a), turn(Culoare::Negru) {
    numarPartideIncepute++;
    stareAnterioara = tabla.getGrila();
}

Joc::~Joc() = default;

//se face o mutare conform regulilor + tratarea mutarilor invalide
void Joc::aplicaMutare(const Mutare& m) {
    //pass
    if (m.isPass()) {
        turn = (turn == Culoare::Negru) ? Culoare::Alb : Culoare::Negru;
        return;
    }
    //loc ocupat
    if (!tabla.esteGol(m.getPozitie())) {
        throw MutareIlegalaException("Pozitia este deja ocupata!");
    }

    const auto grilaInainte = tabla.getGrila();
    tabla.Plaseazapiatra(m.getPozitie(), turn);

    verificaCapturi(m.getPozitie());
    //ko
    if (tabla.getGrila() == stareAnterioara) {
        tabla.setGrila(grilaInainte);
        throw MutareIlegalaException("Regula Ko: Repetarea starii nu este permisa!");
    }

    stareAnterioara = grilaInainte;
    turn = (turn == Culoare::Negru) ? Culoare::Alb : Culoare::Negru;
}

//se verifica vecinii
void Joc::verificaCapturi(Pozitie p) {
    const auto adversar = (turn == Culoare::Negru) ? Culoare::Alb : Culoare::Negru;

    for (int i = 0; i < 4; ++i) {
        constexpr int dx[] = {-1, 1, 0, 0};
        constexpr int dy[] = {0, 0, -1, 1};

        const auto nx = static_cast<int>(p.x) + dx[i];
        const auto ny = static_cast<int>(p.y) + dy[i];

        if (nx >= 0 && nx < static_cast<int>(tabla.getMarime()) && ny >= 0 && ny < static_cast<int>(tabla.getMarime())) {
            Pozitie v = {static_cast<unsigned int>(nx), static_cast<unsigned int>(ny)};
            // daca vecinul este adversar, verificam daca intreg grupul lui a ramas fara libertati
            if (tabla.getPozitieCuloare(v) == adversar) {
                std::set<Pozitie> grupAdvers;
                gasesteGrup(tabla, v, adversar, grupAdvers);
                // eliminam piesele din grup si actualizam scorul
                if (!areLibertati(tabla, grupAdvers)) {
                    for (const auto& pos : grupAdvers) {
                        tabla.Plaseazapiatra(pos, Culoare::Gol); // Eliminam piesa
                        if (turn == Culoare::Negru) capturateNegru++; else capturateAlb++;
                    }
                }
            }
        }
    }
}

//determinarea castigatorului in functie de scor si komi
std::string Joc::determinaCastigator() const {
    const auto scorNegru = static_cast<float>(capturateNegru);
    const auto scorAlb = static_cast<float>(capturateAlb) + komi;

    std::stringstream ss;
    ss << std::fixed << std::setprecision(1);
    ss << "Scor: Negru " << static_cast<int>(scorNegru) << " - Alb " << scorAlb;

    return ss.str() + (scorNegru > scorAlb ? "\nCastigator: Negru!" : "\nCastigator: Alb!");
}