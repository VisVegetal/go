#include <utility>
#include <vector>
#include <string>
#include <fstream>
#include <limits>
#include <set>
#include <iostream>
#include <tuple>
#include <algorithm> // pentru std::swap



enum class Culoare{Negru, Alb, Gol};
enum class Dimensiuni{D9x9, D13x13, D19x19};
enum class tipJ{uman};
enum class tipM{plasare, pass}; // tipurile de mutari posibile
//coordonatele x si y

//declarare pentru vizibilitate in clasele joc si reguli
void TestareJoc(std::istream& is, Dimensiuni dim);

// Functie ajutatoare pentru a converti inputul numeric in Dimensiuni
Dimensiuni getDimensiuneFromInput(int alegere) {
    switch (alegere) {
        case 9: return Dimensiuni::D9x9;
        case 13: return Dimensiuni::D13x13;
        case 19:
        default: return Dimensiuni::D19x19; // 19x19 ca valoare implicita
    }
}

struct Pozitie {
    unsigned int x, y;
    bool operator<(const Pozitie& other) const {
        return std::tie(x, y) < std::tie(other.x, other.y);
    }
    bool operator==(const Pozitie& other) const {
        return x == other.x && y == other.y;
    }
};

//afisarea pozitiei (x,y)
std::ostream& operator<<(std::ostream& os, const Pozitie& p) {
    os << "(" << p.x << ", " << p.y << ")";
    return os;
}
//declaratie pentru compunere
class Joc;


//clasa pentru o piatra
class Pietre {
private:
    Culoare culoare;
    Pozitie pozitie;
public:
    explicit Pietre(Culoare culoare_, unsigned int x, unsigned int y)
        : culoare(culoare_), pozitie({x, y}) {}
    friend std::ostream& operator<<(std::ostream& os, const Pietre& p);

    Pietre(const Pietre& other) :
        culoare(other.culoare), pozitie(other.pozitie) {}

    Pietre& operator=(const Pietre& other) {
        if (this != &other) {
            this->culoare = other.culoare;
            this->pozitie = other.pozitie;
        }
        return *this;
    }

    ~Pietre() {}

    //schimbarea culorii, rar folosita in GO
    [[maybe_unused]]void SchimbaCuloarea() {
        if (this->culoare == Culoare::Negru)
            this->culoare = Culoare::Alb;
        else
            this->culoare = Culoare::Negru;
    }
};

std::ostream& operator<<(std::ostream& os, const Pietre& p) {
    std::string culoare_str = (p.culoare == Culoare::Negru ? "Negru" : (p.culoare == Culoare::Alb ? "Alb" : "Gol"));
    os << "Piatra de culoare: " << culoare_str<< " la " <<p.pozitie;
    return os;
}


//clasa pentru tabla
class Tabla {
private:
    Dimensiuni dimensiune;
    std::vector<std::vector<Culoare>> tabla;
    [[nodiscard]]unsigned int getMarime() const {
        if (dimensiune == Dimensiuni::D9x9) return 9;
        if (dimensiune == Dimensiuni::D13x13) return 13;
        return 19;
    }
public:
    explicit Tabla(Dimensiuni dimensiune_)
        : dimensiune(dimensiune_){
        unsigned int n = getMarime();
        tabla.resize(n, std::vector< Culoare>(n, Culoare::Gol));
    }

    friend std::ostream& operator<<(std::ostream& os, const Tabla& t);

    Tabla(const Tabla& other) :
        dimensiune(other.dimensiune), tabla(other.tabla) {}

    Tabla& operator=(const Tabla& other) {
        if (this != &other) {
            this->dimensiune = other.dimensiune;
            this->tabla = other.tabla;
        }
        return *this;
    }

    ~Tabla() {}

    void Plaseazapiatra (const Pozitie& p, Culoare c);
    void ScoatePiatra (const Pozitie& p);
    [[nodiscard]] bool esteGol(const Pozitie& p) const;

    //cititor de stare (grupuri, libertati)
    [[nodiscard]] Culoare getPozitieCuloare(const Pozitie& p) const {
        unsigned int n = getMarime();
        if (p.x < n && p.y < n)
            return tabla[p.x][p.y];
        return Culoare::Gol;
    }
    friend class Joc;
};

//verifica daca o pozitie este goala si in limite
bool Tabla::esteGol(const Pozitie& p) const {
    unsigned int n = getMarime();
    if (p.x < n && p.y < n)
        return tabla[p.x][p.y] == Culoare::Gol;
    return false;
}

//plaseaza o piatra la pozitia p, daca este legal
void Tabla::Plaseazapiatra(const Pozitie &p, Culoare c) {
    unsigned int n = getMarime();

    if (p.x >= n || p.y >= n) {
        return;
    }

    if (esteGol(p))
        this->tabla[p.x][p.y] = c;
    else
        std::cerr<<"Eroare: Pozitia "<<p<<" e deja ocupata.\n";
}
//scoaterea unei pietre (utilizata pentru capturi)
void Tabla::ScoatePiatra(const Pozitie& p) {
    unsigned int n = getMarime();
    if (p.x < n && p.y < n && tabla[p.x][p.y] != Culoare::Gol)
        tabla[p.x][p.y] = Culoare::Gol;
}

//afisarea starii tablei
std::ostream& operator<<(std::ostream& os, const Tabla& t) {
    unsigned int n = t.getMarime();

    os<<"--- Tabla de GO (" << n << "x" << n << ") ---\n";
    os <<"  ";
    for (unsigned int i = 0; i < n; ++i)
        os << i % 10 << " ";
    os << "\n";

    for (unsigned int i = 0; i < n; ++i) {
        os << i % 10 << " ";
        for (unsigned int j = 0; j < n; ++j)
            switch (t.tabla[i][j]) {
                case Culoare::Negru: os << "N "; break;
                case Culoare::Alb: os << "A "; break;
                case Culoare::Gol: os << "# "; break;
            }
        os << "\n";
    }
    os << "---------------------------\n";
    return os;
}

//clasa pentru un jucator
class Jucator {
private:
    std::string nume;
    unsigned int pietreCapturate;
    Culoare piatra_curenta;
    tipJ tip_jucator;
public:
    explicit Jucator(std::string nume_, unsigned int pietreCapturate_, Culoare piatra_curenta_, tipJ tip_jucator_)
        : nume(std::move(nume_)), pietreCapturate(pietreCapturate_), piatra_curenta(piatra_curenta_), tip_jucator(tip_jucator_) {}

    Jucator(const Jucator& other) :
        nume(other.nume),
        pietreCapturate(other.pietreCapturate),
        piatra_curenta(other.piatra_curenta),
        tip_jucator(other.tip_jucator) {}

    Jucator& operator=(const Jucator& other) {
        if (this != &other) {
            this->nume = other.nume;
            this->pietreCapturate = other.pietreCapturate;
            this->piatra_curenta = other.piatra_curenta;
            this->tip_jucator = other.tip_jucator;
        }
        return *this;
    }

    ~Jucator() {}

    friend std::ostream& operator<<(std::ostream& os, const Jucator& j);

    void AdaugaPietreCapturate(unsigned int nr);

    [[nodiscard]] const std::string& getNume() const { return nume; }

    [[maybe_unused]][[nodiscard]] tipJ getTipJucator() const { return tip_jucator; }

    friend class Joc;
};

//adaugare pietre capturate de jucator
void Jucator::AdaugaPietreCapturate(unsigned int nr) {
    if (nr > 0) {
        this->pietreCapturate += nr;
        std::cout<<this->nume<<" a capturat "<< nr << " pietre. Total pietre capturate: "<< this->pietreCapturate<<".\n";
    }
}

//afisarea detaliilor jucatorilor
std::ostream& operator<<(std::ostream& os, const Jucator& j) {
    std::string culoare_str = (j.piatra_curenta == Culoare::Negru ? "Negru" :
                              (j.piatra_curenta == Culoare::Alb ? "Alb" : "Nespecificata"));
    std::string tip_str = "Uman";

    os<<"--- Jucator "<<j.nume<<"---\n"
      <<"Tip: "<<tip_str<<"\n"
      <<"Culoare: "<<culoare_str<<"\n"
      <<"Pietre capturate: "<< j.pietreCapturate<<"\n";

    return os;
}

//clasa mutare
class Mutare{
private:
    Pozitie pozitie;
    tipM tip_m;
public:
    explicit Mutare(Pozitie pozitie_, tipM tip_m_)
        : pozitie(pozitie_), tip_m(tip_m_) {}

    Mutare(const Mutare& other) :
        pozitie(other.pozitie), tip_m(other.tip_m) {}

    Mutare& operator=(const Mutare& other) {
        if (this != &other) {
            this->pozitie = other.pozitie;
            this->tip_m = other.tip_m;
        }
        return *this;
    }

    ~Mutare() {}

    friend std::ostream& operator<<(std::ostream& os, const Mutare& m);

    [[nodiscard]] bool isPass() const;

    [[nodiscard]] Pozitie getPozitie() const {
        return pozitie;
    }
};

//verifica daca mutarea e de tip pass
bool Mutare::isPass() const {
    return tip_m == tipM::pass;
}

//afisarea detaliilor mutarii
std::ostream& operator<<(std::ostream& os, const Mutare& m) {
    if (m.tip_m == tipM::pass)
        os<<"Mutare: Pass";
    else
        os<<"Mutare la " << m.pozitie;
    return os;
}

//clasa grup
class Grup {
private:
    std::vector<Pozitie> pozitiiPietre;
    std::set<Pozitie> libertati;
    bool capturat;
    Culoare culoare;
    int id;
    public:
        explicit Grup(std::vector<Pozitie> pozitiiPietre_, std::set<Pozitie> libertati_, bool capturat_, Culoare culoare_, int id_)
            : pozitiiPietre(std::move(pozitiiPietre_)), libertati(std::move(libertati_)), capturat(capturat_), culoare(culoare_), id(id_){}

    Grup(const Grup& other) :
        pozitiiPietre(other.pozitiiPietre),
        libertati(other.libertati),
        capturat(other.capturat),
        culoare(other.culoare),
        id(other.id) {}

    Grup& operator=(const Grup& other) {
        if (this != &other) {
            this->pozitiiPietre = other.pozitiiPietre;
            this->libertati = other.libertati;
            this->capturat = other.capturat;
            this->culoare = other.culoare;
            this->id = other.id;
        }
        return *this;
    }

    ~Grup() {}

    friend std::ostream& operator<<(std::ostream& os, const Grup& grup);

};


//afisarea detaliilor grupului
std::ostream& operator<<(std::ostream& os, const Grup& grup) {
    os<<"--- Grup ID: "<<grup.id<<"---\n"
      <<"Culoare: "<<(grup.culoare == Culoare::Negru ? "Negru" : "Alb")<<"\n"
      <<"Pietre: "<< grup.pozitiiPietre.size()<<" (" << (grup.capturat ? "CAPTURAT" : "Activ") << ")\n"
      <<"Libertati (total " << grup.libertati.size()<<"): ";

    os<<"{";
    bool first = true;
    for (const auto& pos : grup.libertati) {
        if (!first) os<<", ";
        os<<pos;
        first = false;
    }
    os<<"}\n";
    return os;
}

//clasa pentru reguli

class Reguli {
private:
    bool permiteSuicid, regulaKo, sfarsitJoc;
    float komi;
    Dimensiuni dimensiuneTabla;
    unsigned int passConsecutive;
public:
    explicit Reguli(bool permiteSuicid_, bool regulaKo_, bool sfarsitJoc_, float komi_, Dimensiuni dimensiune_, unsigned int passConsecutive_)
        :permiteSuicid(permiteSuicid_), regulaKo(regulaKo_), sfarsitJoc(sfarsitJoc_), komi(komi_), dimensiuneTabla(dimensiune_), passConsecutive(passConsecutive_) {}

    [[maybe_unused]][[nodiscard]] bool esteSfarsitJoc() const { return sfarsitJoc; }

    Reguli(const Reguli& other) :
        permiteSuicid(other.permiteSuicid),
        regulaKo(other.regulaKo),
        sfarsitJoc(other.sfarsitJoc),
        komi(other.komi),
        dimensiuneTabla(other.dimensiuneTabla),
        passConsecutive(other.passConsecutive) {}

    Reguli& operator=(const Reguli& other) {
        if (this != &other) {
            this->permiteSuicid = other.permiteSuicid;
            this->regulaKo = other.regulaKo;
            this->sfarsitJoc = other.sfarsitJoc;
            this->komi = other.komi;
            this->dimensiuneTabla = other.dimensiuneTabla;
            this->passConsecutive = other.passConsecutive;
        }
        return *this;
    }

    ~Reguli() {}

    friend std::ostream& operator<<(std::ostream& os, const Reguli& r);

    [[nodiscard]] bool isMutareValida(const Tabla& tabla, const Mutare& mutare) const;

    friend class Joc;
    friend void TestareJoc(std::istream& is, Dimensiuni dim);
};


bool Reguli::isMutareValida(const Tabla &tabla, const Mutare &mutare) const {
    if (this->sfarsitJoc) {
        std::cerr<<"Mutare invalida. Sfarsit joc\n";
        return false;
    }
    if (mutare.isPass())
        return true;

    Pozitie p = mutare.getPozitie();

    if (!tabla.esteGol(p)) {
        std::cerr<<"Mutare invalida, pozitia "<<p<<" este ocupata.\n";
        return false;
    }

    return true;
}
//afisarea setarilor si starilor regulamentare ale jocului
std::ostream& operator<<(std::ostream& os, const Reguli& r) {
    std::string dim_str = (r.dimensiuneTabla == Dimensiuni::D9x9 ? "9x9" :
                           (r.dimensiuneTabla == Dimensiuni::D13x13 ? "13x13" : "19x19"));
    os << "--- Regulile Jocului ---\n"
       << "  Dimensiune Tabla: " << dim_str << "\n"
       << "  Komi: " << r.komi << "\n"
       << "  Permite Suicid: " << (r.permiteSuicid ? "DA" : "NU") << "\n"
       << "  Regula Ko: " << (r.regulaKo ? "ON" : "OFF") << "\n"
       << "  Sfarsit Joc: " << (r.sfarsitJoc ? "DA" : "NU") << "\n"
       << "  Pass-uri Consec.: " << r.passConsecutive << "\n";
    return os;
}

//pentru a stoca o stare a tablei si a implementa regula Ko
struct IstoricTabla {
    std::vector<std::vector<Culoare>> stareaTabla;
    bool operator==(const IstoricTabla& other) const {
        return stareaTabla == other.stareaTabla;
    }
};
//clasa pentru un joc
class Joc {
private:
    Tabla tabla;
    Jucator jucatorNegru, jucatorAlb;
    Reguli reguli;
    Culoare jucatorCurent;
    unsigned int mutariEfectuate;
    std::vector<IstoricTabla> istoricStari{};

    [[nodiscard]] std::set<Pozitie> gasesteGrup(const Pozitie& p) const;
    [[nodiscard]] std::set<Pozitie> gasesteLibertati(const std::set<Pozitie>& grup) const;
    void stergeGrup(const std::set<Pozitie>& grup);

    void AnalizeazaTeritoriu(float& teritoriuNegru, float& teritoriuAlb) const;

    [[nodiscard]] IstoricTabla getStareCurentaTabla() const {
        IstoricTabla stare;
        stare.stareaTabla = tabla.tabla;
        return stare;
    }

public:
    explicit Joc(Dimensiuni dim,
                 const std::string& numeN, Culoare culoareN, tipJ tipN,
                 const std::string& numeA, Culoare culoareA, tipJ tipA,
                 float komi, bool permiteSuicid, bool regulaKo);

    Joc(const Joc& other) :
        tabla(other.tabla),
        jucatorNegru(other.jucatorNegru),
        jucatorAlb(other.jucatorAlb),
        reguli(other.reguli),
        jucatorCurent(other.jucatorCurent),
        mutariEfectuate(other.mutariEfectuate),
        istoricStari(other.istoricStari) {}

    Joc& operator=(const Joc& other) {
        if (this != &other) {
            Joc temp = other;
            std::swap(*this, temp);
        }
        return *this;
    }

    ~Joc() {}

    friend std::ostream& operator<<(std::ostream& os, const Joc& j);

    bool AplicaMutare(const Mutare& m);
    void CalculeazaScorFinal() const;
    void AfiseazaStareaJocului() const;

    [[maybe_unused]][[nodiscard]] std::set<Pozitie> PublicGasesteGrup(const Pozitie& p) const {
        return gasesteGrup(p);
    }

    friend void TestareJoc(std::istream& is, Dimensiuni dim);
};

void Joc::AnalizeazaTeritoriu(float& teritoriuNegru, float& teritoriuAlb) const {
    unsigned int n = tabla.getMarime();
    std::vector<std::vector<bool>> vizitat(n, std::vector<bool>(n, false));

    for (unsigned int i = 0; i < n; ++i) {
        for (unsigned int j = 0; j < n; ++j) {
            Pozitie p = {i, j};

            if (tabla.getPozitieCuloare(p) == Culoare::Gol && !vizitat[i][j]) {

                // 1. Gaseste un grup (Ochi) de spatii goale conectate
                std::set<Pozitie> zonaGol;
                std::vector<Pozitie> coada;
                coada.push_back(p);
                vizitat[i][j] = true;

                std::set<Culoare> culoriApropiate; // Culoarea pietrelor care inconjoara zona
                unsigned int puncteZona = 0;

                while (!coada.empty()) {
                    Pozitie curenta = coada.back();
                    coada.pop_back();
                    zonaGol.insert(curenta);
                    puncteZona++;

                    std::vector<Pozitie> vecini = {
                        {curenta.x, curenta.y - 1}, {curenta.x, curenta.y + 1},
                        {curenta.x - 1, curenta.y}, {curenta.x + 1, curenta.y}
                    };

                    for (const auto& vecin : vecini) {
                        if (vecin.x < n && vecin.y < n) {
                            Culoare culoareVecin = tabla.getPozitieCuloare(vecin);

                            if (culoareVecin == Culoare::Gol) {
                                if (!vizitat[vecin.x][vecin.y]) {
                                    vizitat[vecin.x][vecin.y] = true;
                                    coada.push_back(vecin);
                                }
                            } else {
                                culoriApropiate.insert(culoareVecin);
                            }
                        }
                    }
                }

                // determina proprietarul teritoriului
                if (culoriApropiate.size() == 1) {
                    Culoare proprietar = *culoriApropiate.begin();
                    if (proprietar == Culoare::Negru) {
                        teritoriuNegru += static_cast<float>(puncteZona);
                    } else if (proprietar == Culoare::Alb) {
                        teritoriuAlb += static_cast<float>(puncteZona);
                    }
                }
                // daca culoriApropiate.size() > 1 (e inconjurat de N si A) => teritoriu neutru (dama)
            }
        }
    }
}

Joc::Joc(Dimensiuni dim,
         const std::string& numeN, Culoare culoareN, tipJ tipN,
         const std::string& numeA, Culoare culoareA, tipJ tipA,
         float komi, bool permiteSuicid, bool regulaKo)
    : tabla(dim),
      jucatorNegru(numeN, 0, culoareN, tipN),
      jucatorAlb(numeA, 0, culoareA, tipA),
      reguli(permiteSuicid, regulaKo, false, komi, dim, 0),
      jucatorCurent(Culoare::Negru),
      mutariEfectuate(0)
{}

std::ostream& operator<<(std::ostream& os, const Joc& j) {
        os << "========== STAREA JOCULUI (" << (j.jucatorCurent == Culoare::Negru ? "Negru" : "Alb") << " la mutare) ==========\n";
        os << j.tabla;
        os << j.jucatorNegru;
        os << j.jucatorAlb;
        os << j.reguli;
        os << "Mutari Efectuate: " << j.mutariEfectuate << "\n";
        os << "=================================================================\n";
        return os;
    }

//afiseaza starea jocului
void Joc::AfiseazaStareaJocului() const {
        std::cout << *this;
    }
//gaseste un grup de pietre conectate
std::set<Pozitie> Joc::gasesteGrup(const Pozitie& p) const {
        std::set<Pozitie> grup;
        std::vector<Pozitie> coada;
        Culoare culoareGrup = tabla.getPozitieCuloare(p);

        if (culoareGrup == Culoare::Gol) return grup;

        coada.push_back(p);
        grup.insert(p);

        unsigned int n = tabla.getMarime();

        while (!coada.empty()) {
            Pozitie curenta = coada.back();
            coada.pop_back();

            std::vector<Pozitie> vecini = {
                {curenta.x, curenta.y - 1}, {curenta.x, curenta.y + 1},
                {curenta.x - 1, curenta.y}, {curenta.x + 1, curenta.y}
            };

            for (const auto& vecin : vecini) {
                if (vecin.x < n && vecin.y < n &&
                    tabla.getPozitieCuloare(vecin) == culoareGrup &&
                    grup.find(vecin) == grup.end()) {
                    grup.insert(vecin);
                    coada.push_back(vecin);
                }
            }
        }
        return grup;
    }

//calculreaza spatiile goale(libertati) adiacente unui grup
std::set<Pozitie> Joc::gasesteLibertati(const std::set<Pozitie>& grup) const {
    std::set<Pozitie> libertati;
    unsigned int n = tabla.getMarime();

    for (const auto& p : grup) {
        std::vector<Pozitie> vecini = {
            {p.x, p.y - 1}, {p.x, p.y + 1},
            {p.x - 1, p.y}, {p.x + 1, p.y}
        };

        for (const auto& vecin : vecini) {
            if (vecin.x < n && vecin.y < n && tabla.getPozitieCuloare(vecin) == Culoare::Gol) {
                libertati.insert(vecin);
            }
        }
    }
    return libertati;
}
//marcheaza toate pietrele dintr-un grup ca goale pe tabla
void Joc::stergeGrup(const std::set<Pozitie>& grup) {
        for (const auto& p : grup)
            tabla.ScoatePiatra(p);
    }

//captura si sinucidere
bool Joc::AplicaMutare(const Mutare& m) {
    Culoare culoareCurenta = jucatorCurent;
    Culoare culoareInamica = (jucatorCurent == Culoare::Negru) ? Culoare::Alb : Culoare::Negru;

    if (!reguli.isMutareValida(tabla, m)) return false;

    if (m.isPass()) {
        std::cout << (jucatorCurent == Culoare::Negru ? jucatorNegru.getNume() : jucatorAlb.getNume()) << " a pasat.\n";
        reguli.passConsecutive++;
    }
    else {
        IstoricTabla stareAnterioara = getStareCurentaTabla();

        Pozitie p = m.getPozitie();

        tabla.Plaseazapiatra(p, culoareCurenta);

        size_t capturiTotale = 0;
        std::vector<Pozitie> vecini = {
            {p.x, p.y -1}, {p.x, p.y + 1}, {p.x - 1, p.y}, {p.x + 1, p.y}
        };

        std::vector<std::set<Pozitie>> grupuriCapturate;

        for (const auto& vecin : vecini) {
            if (tabla.getPozitieCuloare(vecin) == culoareInamica) {
                std::set<Pozitie> grupInamic = gasesteGrup(vecin);

                if (gasesteLibertati(grupInamic).empty()) {
                    grupuriCapturate.push_back(grupInamic);
                }
            }
        }


        for (const auto& grupCapturat : grupuriCapturate) {
            capturiTotale += grupCapturat.size();
            stergeGrup(grupCapturat);
            // Mesaj mai explicit despre captura
            std::cout << "--- CAPTURA! --- Grup inamic de " << grupCapturat.size() << " pietre capturat la " << p << ".\n";
        }

        // verifica sinuciderea
        std::set<Pozitie> grupCurent = gasesteGrup(p);
        if (gasesteLibertati(grupCurent).empty() && capturiTotale == 0) {
            if (!reguli.permiteSuicid) {
                tabla.ScoatePiatra(p);
                std::cerr << "Mutare invalida la " << p << ": Sinucidere interzisa! Piatra a fost retrasa.\n";
                return false;
            }
            // Daca sinuciderea este permisa, ar trebui scoasa piatra
            // Dar in regulile de Go moderne, piatra pusa se scoate
            if(reguli.permiteSuicid) {
                tabla.ScoatePiatra(p);
                std::cout << "ATENTIE: Sinucidere! Piatra de la " << p << " a fost scoasa. Mutare permisa prin reguli.\n";
            }
        }

        if (reguli.regulaKo && capturiTotale > 0) {
            IstoricTabla stareCurenta = getStareCurentaTabla();
            if (!istoricStari.empty() && stareCurenta == istoricStari.back()) {
                // anulare KO, reface starea tablei la cum era inainte de mutare
                tabla.tabla = stareAnterioara.stareaTabla;
                std::cerr << "Mutare invalida la " << p << ": INCALCA REGLULA KO! Starea tablei s-ar repeta. Mutare anulata.\n";
                return false;
            }
        }
        if (capturiTotale > 0) {
            if (jucatorCurent == Culoare::Negru)
                jucatorNegru.AdaugaPietreCapturate(capturiTotale);
            else
                jucatorAlb.AdaugaPietreCapturate(capturiTotale);
        }

        istoricStari.push_back(getStareCurentaTabla());

        reguli.passConsecutive = 0;
        mutariEfectuate++;
    }

    jucatorCurent = culoareInamica;
    if (reguli.passConsecutive >= 2) {
        reguli.sfarsitJoc = true;
        std::cout << "Jocul s-a incheiat prin **2 Pass-uri consecutive**!\n";
    }

    return true;
}

//calculare scor final: pietre capturate + teritoriu + komi
void Joc::CalculeazaScorFinal() const {
        auto scorNegru = static_cast<float>(jucatorNegru.pietreCapturate);
        float scorAlb = static_cast<float>(jucatorAlb.pietreCapturate) + reguli.komi;

        float teritoriuNegru = 0.0f;
        float teritoriuAlb = 0.0f;

        AnalizeazaTeritoriu(teritoriuNegru, teritoriuAlb);

        scorNegru += teritoriuNegru;
        scorAlb += teritoriuAlb;

        std::cout << "\n============== CALCUL SCOR FINAL =============\n";
        std::cout << "  **SCOR NEGRU (Pietre + Teritoriu)**\n";
        std::cout << "  - Capturi: "<< jucatorNegru.pietreCapturate << "\n";
        std::cout << "  - Teritoriu: " << teritoriuNegru << "\n";
        std::cout << "\n  **SCOR ALB (Pietre + Komi + Teritoriu)**\n";
        std::cout << "  - Capturi: "<< jucatorAlb.pietreCapturate << "\n";
        std::cout << "  - Komi: " << reguli.komi << "\n";
        std::cout << "  - Teritoriu: " << teritoriuAlb << "\n";

        std::cout<< "\n** SCOR FINAL NEGRU: " << scorNegru << " **\n";
        std::cout << "** SCOR FINAL ALB: " << scorAlb << " **\n";

        if (scorNegru > scorAlb)
            std::cout << "🎉 CASTIGA JUCATORUL NEGRU! 🎉\n";
        else if (scorNegru < scorAlb)
            std::cout << "🎉 CASTIGA JUCATORUL ALB! 🎉\n";
        else
            std::cout << "🤝 REMIZA (JIGO)! 🤝\n";

        std::cout <<"==============================================\n";
    }

// testare joc
void TestareJoc(std::istream& is, Dimensiuni dim) {
    std::string numeN, numeA;
    float komi = 6.5f;

    // Citirea Komi si a numelor jucatorilor
    std::cout << "Introduceti Komi (ex: 6.5): ";
    is >> komi;
    std::cout << "Introduceti numele jucatorului Negru: ";
    is >> numeN;
    std::cout << "Introduceti numele jucatorului Alb: ";
    is >> numeA;

    Joc jocGo(dim, numeN, Culoare::Negru, tipJ::uman, numeA, Culoare::Alb, tipJ::uman, komi, false, true);

    std::string dim_str = (dim == Dimensiuni::D9x9 ? "9x9" : (dim == Dimensiuni::D13x13 ? "13x13" : "19x19"));
    std::cout << "\n============================================\n";
    std::cout << "===== INITIERE JOC Go (" << dim_str << ") =====";
    std::cout << "\n============================================\n";
    jocGo.AfiseazaStareaJocului();

    std::string mutare_tip_str;
    int x, y;
    unsigned int mutare_nr = 1;

    std::cout << "\n===== SIMULARE MUTARI (AplicaMutare) =====\n";
    std::cout << "Formatul mutarilor: **PLASEAZA x y** (ex: PLASEAZA 3 3) sau **PASS**\n\n";

    while (true) {
        std::cout << "Mutarea " << mutare_nr << " (" << (jocGo.jucatorCurent == Culoare::Negru ? jocGo.jucatorNegru.getNume() : jocGo.jucatorAlb.getNume()) << "): ";

        if (!(is >> mutare_tip_str)) break;

        Mutare m({0, 0}, tipM::pass);
        std::string jucator = (jocGo.jucatorCurent == Culoare::Negru ? jocGo.jucatorNegru.getNume() : jocGo.jucatorAlb.getNume());
        std::string culoare = (jocGo.jucatorCurent == Culoare::Negru ? "Negru (N)" : "Alb (A)");

        std::cout << "--- MUTAREA "<< mutare_nr << " - Jucator: **" << jucator << "** (" << culoare << ") ---\n";

        if (mutare_tip_str == "PASS") {
            std::cout << "> Jucatorul a ales să **Paseze**.\n";
        } else if (mutare_tip_str == "PLASEAZA") {
            if (!(is >> x >> y)) break;
            m = Mutare({static_cast<unsigned int>(x), static_cast<unsigned int>(y)}, tipM::plasare);
            std::cout << "> Plaseaza piatra la coordonatele: **x=" << x << ", y=" << y << "**\n";
        } else {
            std::cout << "> Eroare: Tip de mutare necunoscut '" << mutare_tip_str << "'. Reincercati cu PLASEAZA sau PASS.\n";
            continue;
        }

        jocGo.AplicaMutare(m);


        jocGo.AfiseazaStareaJocului();
        mutare_nr++;

        if (jocGo.reguli.sfarsitJoc) break;
    }

    if(jocGo.reguli.sfarsitJoc)
        jocGo.CalculeazaScorFinal();
    else
        std::cout << "\nJocul s-a oprit inainte de final din cauza lipsei de mutari in intrare.\n";
}

int main() {
    std::cout << "========================================================\n";
    std::cout << "         JOC DE GO - SIMULARE CONSOLA INTERACTIVA\n";
    std::cout << "========================================================\n\n";

    int dim_alegere = 0;
    while (dim_alegere != 9 && dim_alegere != 13 && dim_alegere != 19) {
        std::cout << "Alegeti dimensiunea tablei (Introduceti 9, 13 sau 19): ";
        if (!(std::cin >> dim_alegere)) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            dim_alegere = 0;
            std::cout << "Intrare invalida. Va rugam introduceti doar numarul 9, 13 sau 19.\n";
        }
    }

    Dimensiuni dim = getDimensiuneFromInput(dim_alegere);

    std::cout << "\n--------------------------------------------------------\n";
    std::cout << "Tabla aleasa: **" << dim_alegere << "x" << dim_alegere << "**\n";
    std::cout << "--------------------------------------------------------\n";


    TestareJoc(std::cin, dim);

    std::cerr << "\nExecutie finalizata.\n";

    return 0;
}