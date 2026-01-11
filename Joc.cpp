// Joc.cpp
#include "Joc.hpp"
#include "GoExceptions.hpp"
#include <set>
#include <iomanip>
#include <sstream>
#include <optional>

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

static int distManhattan(Pozitie a, Pozitie b) {
    return static_cast<int>(a.x > b.x ? a.x - b.x : b.x - a.x)
         + static_cast<int>(a.y > b.y ? a.y - b.y : b.y - a.y);
}

//initializare componente joc
Joc::Joc(Dimensiuni dim, Jucator* n, Jucator* a)
    : tabla(dim), negru(n), alb(a), turn(Culoare::Negru) {
    numarPartideIncepute++;
    stareAnterioara = tabla.getGrila();

    istoric.clear();
    istoricIndex = 0;
    salveazaSnapshot();
}

Joc::~Joc() = default;

void Joc::salveazaSnapshot() {
    Snapshot s;
    s.grila = tabla.getGrila();
    s.stareKo = stareAnterioara;
    s.turn = turn;
    s.captN = capturateNegru;
    s.captA = capturateAlb;
    s.jocIncheiat = jocIncheiat;
    s.reguliSfarsit = reguli.getSfarsitJoc();
    s.reguliPass = reguli.getPassConsecutive();

    if (istoricIndex + 1 < istoric.size()) {
        istoric.erase(istoric.begin() + static_cast<long>(istoricIndex + 1), istoric.end());
    }
    istoric.push_back(std::move(s));
    istoricIndex = istoric.size() - 1;
}

void Joc::undo() {
    if (!poateUndo()) return;
    istoricIndex--;

    const auto& s = istoric[istoricIndex];
    tabla.setGrila(s.grila);
    stareAnterioara = s.stareKo;
    turn = s.turn;
    capturateNegru = s.captN;
    capturateAlb = s.captA;
    jocIncheiat = s.jocIncheiat;
    reguli.setState(s.reguliSfarsit, s.reguliPass);
}

void Joc::redo() {
    if (!poateRedo()) return;
    istoricIndex++;

    const auto& s = istoric[istoricIndex];
    tabla.setGrila(s.grila);
    stareAnterioara = s.stareKo;
    turn = s.turn;
    capturateNegru = s.captN;
    capturateAlb = s.captA;
    jocIncheiat = s.jocIncheiat;
    reguli.setState(s.reguliSfarsit, s.reguliPass);
}

std::optional<Pozitie> Joc::sugereazaMutare() const {
    if (jocIncheiat) return std::nullopt;

    const auto n = tabla.getMarime();
    const auto adversar = (turn == Culoare::Negru) ? Culoare::Alb : Culoare::Negru;
    const Pozitie centru{n / 2, n / 2};

    int bestCapt = -1;
    int bestDist = 1'000'000;
    std::optional<Pozitie> best = std::nullopt;

    for (unsigned int x = 0; x < n; ++x) {
        for (unsigned int y = 0; y < n; ++y) {
            Pozitie p{x, y};
            if (!tabla.esteGol(p)) continue;

            Mutare m(p, tipM::plasare);
            if (!reguli.esteMutareValida(tabla, m)) continue;

            Tabla tmp = tabla;
            tmp.Plaseazapiatra(p, turn);

            int capt = 0;
            for (int i = 0; i < 4; ++i) {
                constexpr int dx[] = {-1, 1, 0, 0};
                constexpr int dy[] = {0, 0, -1, 1};

                int nx = static_cast<int>(p.x) + dx[i];
                int ny = static_cast<int>(p.y) + dy[i];
                if (nx < 0 || ny < 0 || nx >= static_cast<int>(n) || ny >= static_cast<int>(n)) continue;

                Pozitie v{static_cast<unsigned int>(nx), static_cast<unsigned int>(ny)};
                if (tmp.getPozitieCuloare(v) != adversar) continue;

                std::set<Pozitie> grupAdvers;
                gasesteGrup(tmp, v, adversar, grupAdvers);
                if (!areLibertati(tmp, grupAdvers)) {
                    capt += static_cast<int>(grupAdvers.size());
                }
            }

            const int d = distManhattan(p, centru);

            if (capt > bestCapt || (capt == bestCapt && d < bestDist)) {
                bestCapt = capt;
                bestDist = d;
                best = p;
            }
        }
    }

    return best;
}

//se face o mutare conform regulilor + tratarea mutarilor invalide
void Joc::aplicaMutare(const Mutare& m) {
    if (!reguli.esteMutareValida(tabla, m)) {
        throw MutareIlegalaException("Mutare invalida conform regulilor!");
    }
    //pass
    if (m.isPass()) {
        reguli.incrementPass();

        // Dacă s-au dat 2 pass-uri consecutive, jocul se termină
        if (reguli.getSfarsitJoc()) {
            this->terminaJoc();
        }

        turn = (turn == Culoare::Negru) ? Culoare::Alb : Culoare::Negru;
        salveazaSnapshot();
        return;
    }
    //loc ocupat
    if (!tabla.esteGol(m.getPozitie())) {
        throw MutareIlegalaException("Pozitia este deja ocupata!");
    }

    reguli.resetPass();

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
    salveazaSnapshot();
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
