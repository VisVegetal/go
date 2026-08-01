#include "../include/GoLogic.hpp"
#include "../include/Tabla.hpp"

namespace GoLogic {

void gasesteGrup(const Tabla& t, Pozitie start, Culoare c, std::set<Pozitie>& grup) {
    const auto n = t.getMarime();
    if (start.x >= n || start.y >= n) return;

    std::stack<Pozitie> stiva;
    stiva.push(start);

    while (!stiva.empty()) {
        const auto p = stiva.top();
        stiva.pop();

        if (p.x >= n || p.y >= n || t.getPozitieCuloare(p) != c || grup.count(p) != 0) {
            continue;
        }

        grup.insert(p);
        for (int i = 0; i < 4; ++i) {
            const auto nx = static_cast<unsigned int>(static_cast<int>(p.x) + dx[i]);
            const auto ny = static_cast<unsigned int>(static_cast<int>(p.y) + dy[i]);
            if (nx < n && ny < n) {
                stiva.push({nx, ny});
            }
        }
    }
}

std::vector<std::set<Pozitie>> gasesteToateGrupele(const Tabla& t, Culoare c) {
    const auto n = t.getMarime();
    std::vector<std::set<Pozitie>> rezultat;
    std::set<Pozitie> vazute;

    for (unsigned int i = 0; i < n; ++i) {
        for (unsigned int j = 0; j < n; ++j) {
            const Pozitie p{i, j};
            if (t.getPozitieCuloare(p) != c || vazute.count(p) != 0) {
                continue;
            }
            std::set<Pozitie> grup;
            gasesteGrup(t, p, c, grup);
            vazute.insert(grup.begin(), grup.end());
            rezultat.push_back(std::move(grup));
        }
    }
    return rezultat;
}

bool areLibertati(const Tabla& t, const std::set<Pozitie>& grup) {
    const auto n = t.getMarime();
    for (const auto& p : grup) {
        for (int i = 0; i < 4; ++i) {
            const auto nx = static_cast<unsigned int>(static_cast<int>(p.x) + dx[i]);
            const auto ny = static_cast<unsigned int>(static_cast<int>(p.y) + dy[i]);
            if (nx < n && ny < n && t.esteGol({nx, ny})) {
                return true;
            }
        }
    }
    return false;
}

unsigned int numarLibertati(const Tabla& t, const std::set<Pozitie>& grup) {
    const auto n = t.getMarime();
    std::set<Pozitie> libertati;

    for (const auto& p : grup) {
        for (int i = 0; i < 4; ++i) {
            const auto nx = static_cast<unsigned int>(static_cast<int>(p.x) + dx[i]);
            const auto ny = static_cast<unsigned int>(static_cast<int>(p.y) + dy[i]);
            if (nx < n && ny < n && t.esteGol({nx, ny})) {
                libertati.insert({nx, ny});
            }
        }
    }
    return static_cast<unsigned int>(libertati.size());
}

int plaseazaCuCapturi(Tabla& t, Pozitie p, Culoare c) {
    const auto n = t.getMarime();
    if (p.x >= n || p.y >= n || !t.esteGol(p)) {
        return 0;
    }

    const auto adversar = (c == Culoare::Negru) ? Culoare::Alb : Culoare::Negru;
    t.Plaseazapiatra(p, c);

    int capturi = 0;
    for (int i = 0; i < 4; ++i) {
        const auto nx = static_cast<unsigned int>(static_cast<int>(p.x) + dx[i]);
        const auto ny = static_cast<unsigned int>(static_cast<int>(p.y) + dy[i]);
        if (nx >= n || ny >= n) {
            continue;
        }

        const Pozitie vecina{nx, ny};
        if (t.getPozitieCuloare(vecina) != adversar) {
            continue;
        }

        std::set<Pozitie> grupAdvers;
        gasesteGrup(t, vecina, adversar, grupAdvers);
        if (!areLibertati(t, grupAdvers)) {
            for (const auto& pos : grupAdvers) {
                t.eliminaPiatra(pos);
            }
            capturi += static_cast<int>(grupAdvers.size());
        }
    }
    return capturi;
}

bool esteSuicidala(const Tabla& t, Pozitie p, Culoare c) {
    Tabla tmp = t;
    plaseazaCuCapturi(tmp, p, c);

    std::set<Pozitie> grup;
    gasesteGrup(tmp, p, c, grup);
    return grup.empty() || !areLibertati(tmp, grup);
}

RezultatSimulare simuleazaPlasare(const Tabla& t, Pozitie p, Culoare c) {
    RezultatSimulare rez{false, 0, 0};
    if (!t.esteGol(p)) {
        return rez;
    }

    Tabla tmp = t;
    rez.capturi = plaseazaCuCapturi(tmp, p, c);

    std::set<Pozitie> grup;
    gasesteGrup(tmp, p, c, grup);
    if (grup.empty() || !areLibertati(tmp, grup)) {
        return rez; // sinucidere
    }

    rez.hash = hashTabla(tmp);
    rez.valida = true;
    return rez;
}

std::size_t hashTabla(const Tabla& t) {
    constexpr std::size_t offset = 1469598103934665603ull; // FNV-1a 64-bit
    constexpr std::size_t prime = 1099511628211ull;

    std::size_t h = offset;
    for (const auto& rand : t.getGrila()) {
        for (const auto celula : rand) {
            h ^= static_cast<std::size_t>(celula);
            h *= prime;
        }
    }
    return h;
}

std::vector<Pozitie> enumeraMutariValide(const Tabla& t, Culoare c,
                                         const std::unordered_set<std::size_t>& koSet) {
    const auto n = t.getMarime();
    std::vector<Pozitie> mutari;

    for (unsigned int i = 0; i < n; ++i) {
        for (unsigned int j = 0; j < n; ++j) {
            const Pozitie p{i, j};
            if (!t.esteGol(p)) {
                continue;
            }

            const auto rez = simuleazaPlasare(t, p, c);
            if (!rez.valida || koSet.count(rez.hash) != 0) {
                continue; // sinucidere sau incalcare Ko
            }
            mutari.push_back(p);
        }
    }
    return mutari;
}

std::vector<MutareCandidat> enumeraCandidati(const Tabla& t, Culoare c,
                                             const std::unordered_set<std::size_t>& koSet) {
    const auto n = t.getMarime();
    std::vector<MutareCandidat> mutari;

    for (unsigned int i = 0; i < n; ++i) {
        for (unsigned int j = 0; j < n; ++j) {
            const Pozitie p{i, j};
            if (!t.esteGol(p)) {
                continue;
            }

            const auto rez = simuleazaPlasare(t, p, c);
            if (!rez.valida || koSet.count(rez.hash) != 0) {
                continue;
            }
            mutari.push_back({p, rez.capturi, rez.hash});
        }
    }
    return mutari;
}

std::pair<int, int> calculeazaTeritoriu(const Tabla& t) {
    const auto n = t.getMarime();
    std::vector<std::vector<bool>> vizitat(n, std::vector<bool>(n, false));

    int teritoriuNegru = 0;
    int teritoriuAlb = 0;

    for (unsigned int i = 0; i < n; ++i) {
        for (unsigned int j = 0; j < n; ++j) {
            const Pozitie start{i, j};
            if (!t.esteGol(start) || vizitat[i][j]) {
                continue;
            }

            // Inundam (flood fill) regiunea goala curenta.
            std::set<Pozitie> regiune;
            bool atinsNegru = false;
            bool atinsAlb = false;

            std::stack<Pozitie> stiva;
            stiva.push(start);
            vizitat[i][j] = true;

            while (!stiva.empty()) {
                const auto cur = stiva.top();
                stiva.pop();

                const auto cul = t.getPozitieCuloare(cur);
                if (cul == Culoare::Gol) {
                    if (regiune.count(cur) == 0) {
                        regiune.insert(cur);
                        for (int k = 0; k < 4; ++k) {
                            const auto nx = static_cast<unsigned int>(static_cast<int>(cur.x) + dx[k]);
                            const auto ny = static_cast<unsigned int>(static_cast<int>(cur.y) + dy[k]);
                            if (nx < n && ny < n && !vizitat[nx][ny]) {
                                vizitat[nx][ny] = true;
                                stiva.push({nx, ny});
                            }
                        }
                    }
                } else if (cul == Culoare::Negru) {
                    atinsNegru = true;
                } else if (cul == Culoare::Alb) {
                    atinsAlb = true;
                }
            }

            if (atinsNegru && !atinsAlb) {
                teritoriuNegru += static_cast<int>(regiune.size());
            } else if (atinsAlb && !atinsNegru) {
                teritoriuAlb += static_cast<int>(regiune.size());
            }
            // Regiune neutra (dame) atinsa de ambele culori: nu se numara.
        }
    }

    return {teritoriuNegru, teritoriuAlb};
}

}
