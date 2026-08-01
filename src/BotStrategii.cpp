#include "../include/BotStrategii.hpp"
#include "../include/GoLogic.hpp"
#include "../include/Joc.hpp"
#include "../include/Tabla.hpp"

#include <algorithm>
#include <optional>
#include <random>
#include <set>
#include <utility>
#include <vector>

namespace {

std::mt19937& rng() {
    static std::mt19937 gen(std::random_device{}());
    return gen;
}

std::optional<Pozitie> alegeRandom(const std::vector<Pozitie>& mutari) {
    if (mutari.empty()) {
        return std::nullopt;
    }
    std::uniform_int_distribution<std::size_t> dis(0, mutari.size() - 1);
    return mutari[dis(rng())];
}

Culoare adversar(Culoare c) {
    return (c == Culoare::Negru) ? Culoare::Alb : Culoare::Negru;
}

constexpr int INF = 1'000'000'000;

// Evaluare pozitie: scorul lui 'mine' minus scorul adversarului
// (teritoriu + pioni pe tabla + 4 * capturi).
int evalueaza(const Tabla& t, Culoare mine, int captMine, int captAdv) {
    const auto [terN, terA] = GoLogic::calculeazaTeritoriu(t);

    int pioniNegru = 0;
    int pioniAlb = 0;
    for (const auto& rand : t.getGrila()) {
        for (const auto celula : rand) {
            if (celula == Culoare::Negru) ++pioniNegru;
            else if (celula == Culoare::Alb) ++pioniAlb;
        }
    }

    int scorMine = 0;
    int scorAdv = 0;
    if (mine == Culoare::Negru) {
        scorMine = terN + pioniNegru + 4 * captMine;
        scorAdv = terA + pioniAlb + 4 * captAdv;
    } else {
        scorMine = terA + pioniAlb + 4 * captMine;
        scorAdv = terN + pioniNegru + 4 * captAdv;
    }
    return scorMine - scorAdv;
}

// Nod de cautare pentru Minimax: copie a tablei + istoric Ko + capturi.
struct Nod {
    Tabla tabla;
    std::unordered_set<std::size_t> koSet;
    int captMine = 0;
    int captAdv = 0;
};

void aplicaCandidat(Nod& nod, const GoLogic::MutareCandidat& cand, Culoare c, Culoare mine) {
    const int capturi = GoLogic::plaseazaCuCapturi(nod.tabla, cand.poz, c);
    if (c == mine) {
        nod.captMine += capturi;
    } else {
        nod.captAdv += capturi;
    }
    nod.koSet.insert(cand.hashDupa);
}

// Ordoneaza candidatii dupa o scorare rapida (capturi, apoi apropiere de centru).
void ordoneazaCandidatii(std::vector<GoLogic::MutareCandidat>& candidati, const Tabla& t) {
    const auto n = t.getMarime();
    const Pozitie centru{n / 2, n / 2};

    std::sort(candidati.begin(), candidati.end(),
              [&](const GoLogic::MutareCandidat& a, const GoLogic::MutareCandidat& b) {
                  const auto scor = [&](const GoLogic::MutareCandidat& c) {
                      const int mx = static_cast<int>(c.poz.x) - static_cast<int>(centru.x);
                      const int my = static_cast<int>(c.poz.y) - static_cast<int>(centru.y);
                      const int md = (mx < 0 ? -mx : mx) + (my < 0 ? -my : my);
                      return c.capturi * 1000 + (40 - md);
                  };
                  return scor(a) > scor(b);
              });

    std::size_t limita = 40;
    if (n > 9) limita = 30;
    if (n > 13) limita = 22;
    if (candidati.size() > limita) {
        candidati.resize(limita);
    }
}

int minimax(Nod& nod, unsigned int adancime, int alpha, int beta, Culoare c, Culoare mine) {
    auto candidati = GoLogic::enumeraCandidati(nod.tabla, c, nod.koSet);

    if (adancime == 0 || candidati.empty()) {
        return evalueaza(nod.tabla, mine, nod.captMine, nod.captAdv);
    }

    ordoneazaCandidatii(candidati, nod.tabla);

    if (c == mine) {
        int best = -INF;
        for (const auto& cand : candidati) {
            Nod copie = nod;
            aplicaCandidat(copie, cand, c, mine);
            best = std::max(best, minimax(copie, adancime - 1, alpha, beta, adversar(c), mine));
            alpha = std::max(alpha, best);
            if (beta <= alpha) break;
        }
        return best;
    }

    int best = INF;
    for (const auto& cand : candidati) {
        Nod copie = nod;
        aplicaCandidat(copie, cand, c, mine);
        best = std::min(best, minimax(copie, adancime - 1, alpha, beta, adversar(c), mine));
        beta = std::min(beta, best);
        if (beta <= alpha) break;
    }
    return best;
}

} // namespace anonim

// --------------------------- BotUsor ---------------------------

std::unique_ptr<IBotStrategy> BotUsor::clone() const {
    return std::make_unique<BotUsor>(*this);
}

Mutare BotUsor::alegeMutare(const Joc& joc) const {
    const auto mutari = GoLogic::enumeraMutariValide(
        joc.getTabla(), joc.getTurnActual(), joc.getStariKo());

    const auto p = alegeRandom(mutari);
    if (!p.has_value()) {
        return Mutare({0, 0}, tipM::pass);
    }
    return Mutare(*p, tipM::plasare);
}

// --------------------------- BotMediu ---------------------------

std::unique_ptr<IBotStrategy> BotMediu::clone() const {
    return std::make_unique<BotMediu>(*this);
}

Mutare BotMediu::alegeMutare(const Joc& joc) const {
    const auto c = joc.getTurnActual();
    const auto adversarul = adversar(c);
    const auto& tabla = joc.getTabla();
    const auto& koSet = joc.getStariKo();
    const auto n = tabla.getMarime();

    std::vector<Pozitie> mutariCapturi;
    std::vector<Pozitie> mutariSalvare;      // grupuri proprii in Atari
    std::vector<Pozitie> mutariLangaAdversar;

    for (unsigned int i = 0; i < n; ++i) {
        for (unsigned int j = 0; j < n; ++j) {
            const Pozitie p{i, j};
            if (!tabla.esteGol(p)) {
                continue;
            }

            const auto rez = GoLogic::simuleazaPlasare(tabla, p, c);
            if (!rez.valida || koSet.count(rez.hash) != 0) {
                continue;
            }

            if (rez.capturi > 0) {
                mutariCapturi.push_back(p);
            }

            bool eSalvare = false;
            bool langaAdversar = false;
            for (int k = 0; k < 4; ++k) {
                const auto nx = static_cast<unsigned int>(static_cast<int>(i) + GoLogic::dx[k]);
                const auto ny = static_cast<unsigned int>(static_cast<int>(j) + GoLogic::dy[k]);
                if (nx >= n || ny >= n) {
                    continue;
                }

                const Pozitie vecina{nx, ny};
                const auto cul = tabla.getPozitieCuloare(vecina);

                if (cul == adversarul) {
                    langaAdversar = true;
                } else if (cul == c) {
                    std::set<Pozitie> grup;
                    GoLogic::gasesteGrup(tabla, vecina, c, grup);
                    if (GoLogic::numarLibertati(tabla, grup) == 1) {
                        eSalvare = true; // grup propriu in Atari
                    }
                }
            }

            if (eSalvare) mutariSalvare.push_back(p);
            if (langaAdversar) mutariLangaAdversar.push_back(p);
        }
    }

    // 1) Prioritizeaza capturile; dintre acestea, alege una cu capturi maxime.
    if (!mutariCapturi.empty()) {
        int maxCapturi = 0;
        for (const auto& p : mutariCapturi) {
            const auto rez = GoLogic::simuleazaPlasare(tabla, p, c);
            maxCapturi = std::max(maxCapturi, rez.capturi);
        }

        std::vector<Pozitie> celeMaiBune;
        for (const auto& p : mutariCapturi) {
            const auto rez = GoLogic::simuleazaPlasare(tabla, p, c);
            if (rez.capturi == maxCapturi) {
                celeMaiBune.push_back(p);
            }
        }
        if (auto p = alegeRandom(celeMaiBune)) {
            return Mutare(*p, tipM::plasare);
        }
    }

    // 2) Salveaza grupurile proprii cu o singura libertate.
    if (auto p = alegeRandom(mutariSalvare)) {
        return Mutare(*p, tipM::plasare);
    }

    // 3) Joaca langa piesele adversarului.
    if (auto p = alegeRandom(mutariLangaAdversar)) {
        return Mutare(*p, tipM::plasare);
    }

    // Fallback: orice mutare valida (sau pass daca tabla e plina/blocata).
    const auto mutari = GoLogic::enumeraMutariValide(tabla, c, koSet);
    if (auto p = alegeRandom(mutari)) {
        return Mutare(*p, tipM::plasare);
    }
    return Mutare({0, 0}, tipM::pass);
}

// --------------------------- BotAvansat ---------------------------

BotAvansat::BotAvansat(unsigned int adancime) : adancime_(adancime) {}

std::unique_ptr<IBotStrategy> BotAvansat::clone() const {
    return std::make_unique<BotAvansat>(*this);
}

Mutare BotAvansat::alegeMutare(const Joc& joc) const {
    const auto mine = joc.getTurnActual();
    const auto& tabla = joc.getTabla();
    const auto n = tabla.getMarime();

    // Daca tabla e goala, deschide aproape de centru (stil standard).
    bool tablaGoala = true;
    for (const auto& rand : tabla.getGrila()) {
        for (const auto celula : rand) {
            if (celula != Culoare::Gol) {
                tablaGoala = false;
                break;
            }
        }
        if (!tablaGoala) break;
    }
    if (tablaGoala) {
        const Pozitie centru{n / 2, n / 2};
        return Mutare(centru, tipM::plasare);
    }

    Nod radacina{tabla, joc.getStariKo(),
                 (mine == Culoare::Negru) ? joc.getCapturateNegru() : joc.getCapturateAlb(),
                 (mine == Culoare::Negru) ? joc.getCapturateAlb() : joc.getCapturateNegru()};

    auto candidati = GoLogic::enumeraCandidati(radacina.tabla, mine, radacina.koSet);
    if (candidati.empty()) {
        return Mutare({0, 0}, tipM::pass);
    }
    ordoneazaCandidatii(candidati, radacina.tabla);

    int bestVal = -INF;
    std::vector<GoLogic::MutareCandidat> celeMaiBune;

    for (const auto& cand : candidati) {
        Nod copie = radacina;
        aplicaCandidat(copie, cand, mine, mine);

        const int val = minimax(copie, adancime_ - 1, -INF, INF, adversar(mine), mine);
        if (val > bestVal) {
            bestVal = val;
            celeMaiBune.clear();
            celeMaiBune.push_back(cand);
        } else if (val == bestVal) {
            celeMaiBune.push_back(cand);
        }
    }

    if (celeMaiBune.empty()) {
        return Mutare({0, 0}, tipM::pass);
    }

    std::uniform_int_distribution<std::size_t> dis(0, celeMaiBune.size() - 1);
    return Mutare(celeMaiBune[dis(rng())].poz, tipM::plasare);
}

// --------------------------- Factory ---------------------------

std::unique_ptr<IBotStrategy> creeazaStrategie(NivelBot nivel) {
    switch (nivel) {
        case NivelBot::Usor:
            return std::make_unique<BotUsor>();
        case NivelBot::Avansat:
            return std::make_unique<BotAvansat>();
        case NivelBot::Mediu:
        default:
            return std::make_unique<BotMediu>();
    }
}
