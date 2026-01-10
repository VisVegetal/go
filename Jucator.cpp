#include "Jucator.hpp"
#include "Tabla.hpp"
#include <vector>
#include <random>


Mutare JucatorUman::alegeMutare(const Tabla&) {
    return Mutare({0, 0}, tipM::pass);
}


Mutare JucatorBot::alegeMutare(const Tabla& t) {
    const auto n = t.getMarime();
    Culoare adversar = (culoare == Culoare::Alb) ? Culoare::Negru : Culoare::Alb;

    // categorii pentru mutarile posibile în funcție de prioritate
    std::vector<Pozitie> mutariAgresive;
    std::vector<Pozitie> mutariSalvare;
    std::vector<Pozitie> toateLibere;

    // se evalueaza punctele libere
    for (unsigned int i = 0; i < n; ++i) {
        for (unsigned int j = 0; j < n; ++j) {
            Pozitie p = {i, j};

            if (t.esteGol(p)) {
                toateLibere.push_back(p);

                constexpr int dx[] = {-1, 1, 0, 0}; //NOLINT
                constexpr int dy[] = {0, 0, -1, 1}; //NOLINT

                for(int k = 0; k < 4; k++) {

                    const auto ni = static_cast<int>(i) + dx[k];
                    const auto nj = static_cast<int>(j) + dy[k];

                    // verificam daca vecinul se afla în interiorul tablei
                    if (ni >= 0 && ni < static_cast<int>(n) && nj >= 0 && nj < static_cast<int>(n)) {
                        Pozitie vecina = {static_cast<unsigned int>(ni), static_cast<unsigned int>(nj)};
                        Culoare culVecin = t.getPozitieCuloare(vecina);

                        if (culVecin == adversar) {
                            mutariAgresive.push_back(p); //mutare langa adversar - atac
                        }
                        if (culVecin == culoare) {
                            mutariSalvare.push_back(p); //mutare langa propriile piese - aparare
                        }
                    }
                }
            }
        }
    }

    // generare de numere random pt a mentine starea secventei intre apeluri
    static std::mt19937 gen(std::random_device{}());

    //Ierarhia Prioritatilor

    // 1. prioritate maxima: atacul
    if (!mutariAgresive.empty()) {
        std::uniform_int_distribution<size_t> dis(0, mutariAgresive.size() - 1);
        return {mutariAgresive[dis(gen)], tipM::plasare};
    }

    // 2. prioritate secundara: apararea
    if (!mutariSalvare.empty()) {
        std::uniform_int_distribution<size_t> dis(0, mutariSalvare.size() - 1);
        return {mutariSalvare[dis(gen)], tipM::plasare};
    }

    // 3. opțiune de rezerva: arice loc liber ales aleatoriu pentru a mentine prezența pe tabla
    if (!toateLibere.empty()) {
        std::uniform_int_distribution<size_t> dis(0, toateLibere.size() - 1);
        return {toateLibere[dis(gen)], tipM::plasare};
    }
    //pass
    return Mutare({0, 0}, tipM::pass);
}