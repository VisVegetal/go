#include <vector>
#include <array>
#include <utility>
#include <string>
#include <fstream>
#include <set>
#include <iostream>
#include <algorithm>


enum class Culoare{Negru, Alb, Gol};
enum class Dimensiuni{D9x9, D13x13, D19x19};
enum class tipJ{uman, AI};
enum class tipM{plasare, pass};

struct Pozitie {
    unsigned int x, y;
    bool operator<(const Pozitie& other) const {
        return std::tie(x, y) < std::tie(other.x, other.y);
    }
    bool operator==(const Pozitie& other) const {
        return x == other.x && y == other.y;
    }
};

std::ostream& operator<<(std::ostream& os, const Pozitie& p) {
    os << "(" << p.x << ", " << p.y << ")";
    return os;
}

class Joc; //declarare inainte necesara pentru friend class

class Pietre {
private:
    Culoare culoare;
    Pozitie pozitie; // coordonatele (x, y)
public:
    explicit Pietre(Culoare culoare_, unsigned int x, unsigned int y)
        : culoare(culoare_), pozitie({x, y}) {}
    friend std::ostream& operator<<(std::ostream& os, const Pietre& p);// debug

    //Regula celor Trei (Constructor de Copiere, Operator=, Destructor), pentru ca nu aloc dinamic
    Pietre(const Pietre& other) = default;
    Pietre& operator=(const Pietre& other) = default;
    ~Pietre() = default;

    void SchimbaCuloarea() {
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

class Tabla {
private:
    Dimensiuni dimensiune;
    std::vector<std::vector<Culoare>> tabla;
    unsigned int getMarime() const {
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

    Tabla(const Tabla& other) = default;
    Tabla& operator=(const Tabla& other) = default;
    ~Tabla() = default;

    void Plaseazapiatra (const Pozitie& p, Culoare c);
    void ScoatePiatra (const Pozitie& p);
    bool esteGol(const Pozitie& p) const;

    Culoare getPozitieCuloare(const Pozitie& p) const {
        unsigned int n = getMarime();
        if (p.x < n && p.y < n)
            return tabla[p.x][p.y];
        return Culoare::Gol;
    }
    friend class Joc;
};

bool Tabla::esteGol(const Pozitie& p) const {
    unsigned int n = getMarime();
    if (p.x < n && p.y < n)
        return tabla[p.x][p.y] == Culoare::Gol;
    return false;
}

void Tabla::Plaseazapiatra(const Pozitie &p, Culoare c) {
    unsigned int n = getMarime();

    if (p.x >= n || p.y >= n) {
        std::cerr << "Eroare: Pozitia " << p << " este in afara limitelor (" << n << "x" << n << ").\n";
        return;
    }

    if (esteGol(p))
        this->tabla[p.x][p.y] = c;
    else
        std::cerr<<"Eroare: Pozitia "<<p<<"e deja ocupata.\n";
}

void Tabla::ScoatePiatra(const Pozitie& p) {
    unsigned int n = getMarime();
    if (p.x < n && p.y < n && && tabla[p.x][p.y] != Culoare::Gol)
        tabla[p.x][p.y] = Culoare::Gol;
}

std::ostream& operator<<(std::ostream& os, const Tabla& t) {
    unsigned int n = t.getMarime();

    os<<"---Tabla de GO(" << n << "x" << n << ")---\n";
    os <<" ";
    for (unsigned int i = 0; i < n; ++i)
        os << i % 10 << " ";
    os << "\n";

    for (unsigned int i = 0; i < n; ++i) {
        os << i % 10 << " ";
        for (unsigned int j = 0; j < n; ++j)
            switch (t.tabla[i][j]) {
                case Culoare::Negru: os << "N"; break;
                case Culoare::Alb: os << "A"; break;
                case Culoare::Gol: os << "#"; break;
            }
        os << "\n";
    }
    os << "----------------------\n";
    return os;
}


class Jucator {
private:
    std::string nume;
    unsigned int pietreCapturate;
    Culoare piatra_curenta; //culoarea pietrei curente
    tipJ tip_jucator;
public:
    explicit Jucator(std::string nume_, unsigned int pietreCapturate_, Culoare piatra_curenta_, tipJ tip_jucator_)
        : nume(nume_), pietreCapturate(pietreCapturate_), piatra_curenta(piatra_curenta_), tip_jucator(tip_jucator_) {}

    Jucator(const Jucator& other) = default;
    Jucator& operator=(const Jucator& other) = default;
    ~Jucator() = default;

    friend std::ostream& operator<<(std::ostream& os, const Jucator& j);

    void AdaugaPietreCapturate(unsigned int nr);

    friend class Joc;
};

void Jucator::AdaugaPietreCapturate(unsigned int nr) {
    if (nr > 0) {
        this->pietreCapturate += nr;
        std::cout<<this->nume<<"a capturat"<< nr << "pietre. Total pietre capturate: "<< this->pietreCapturate<<".\n";
    }
}

std::ostream& operator<<(std::ostream& os, const Jucator& j) {
    std::string culoare_str = (j.piatra_curenta == Culoare::Negru ? "Negru" :
                              (j.piatra_curenta == Culoare::Alb ? "Alb" : "Nespecificata"));
    std::string tip_str = (j.tip_jucator == tipJ::uman ? "Uman" : "AI");

    os<<"---Jucator "<<j.nume<<"---\n"
      <<"Tip: "<<tip_str<<"\n"
      <<"Culoare: "<<culoare_str<<"\n"
      <<"Pietre capturate: "<< j.pietreCapturate<<"\n";

    return os;
}

class Mutare{
private:
    Pozitie pozitie; //unde muta
    tipM tip_m;
public:
    explicit Mutare(Pozitie pozitie_, tipM tip_m_)
        : pozitie(pozitie_), tip_m(tip_m_) {}

    Mutare(const Mutare& other) = default;
    Mutare& operator=(const Mutare& other) = default;
    ~Mutare() = default;

    friend std::ostream& operator<<(std::ostream& os, const Mutare& m);

    bool isPass() const;

    Pozitie getPozitie() const {
        return pozitie;
}
};

bool Mutare::isPass() const {
    return tip_m == tipM::pass;
}

std::ostream& operator<<(std::ostream& os, const Mutare& m) {
    if (m.tip_m == tipM::pass)
        os<<"Mutare: Pass";
    else
        os<<"Mutare la " << m.pozitie;
    return os;
}

class Grup {
private:
    std::vector<Pozitie> pozitiiPietre;
    std::set<Pozitie> libertati;
    bool capturat;
    Culoare culoare;
    int id;
    public:
        explicit Grup(std::vector<Pozitie> pozitiiPietre_, std::set<Pozitie> libertati_, bool capturat_, Culoare culoare_, int id_)
            : pozitiiPietre(pozitiiPietre_), libertati(libertati_), capturat(capturat_), culoare(culoare_), id(id_){}

    Grup(const Grup& other) = default;
    Grup& operator=(const Grup& other) = default;
    ~Grup() = default;

    friend std::ostream& operator<<(std::ostream& os, const Grup& grup);

    void AdaugaLibertate(const Pozitie& p);
};

void Grup::AdaugaLibertate(const Pozitie& p) {
    auto[iterator, inserat] = this->libertati.insert(p);
    if (inserat)
        std::cout<<"Libertate adaugata la: "<< p << "pentru grupul: "<< id << ".\n";
    else
        std::cerr<<"Libertatea "<<p<<"exista deja in Grupul "<< id << ".\n";
}
std::ostream& operator<<(std::ostream& os, const Grup& grup) {
    os<<"---Grup ID: "<<grup.id<<"---\n"
      <<"Culoare: "<<(grup.culoare == Culoare::Negru ? "Negru" : "Alb")<<"\n"
      <<"Pietre: "<< grup.pozitiiPietre.size()<<" (" << (grup.capturat ? "CAPTURAT" : "Activ") << ")\n"
      <<"Libertati (total" << grup.libertati.size()<<"): ";

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
class Reguli {
private:
    bool permiteSuicid, regulaKo, sfarsitJoc;
    float komi;
    Dimensiuni dimensiuneTabla;
    unsigned int passConsecutive;
    public:
        explicit Reguli(bool permiteSuicid_, bool regulaKo_, bool sfarsitJoc_, float komi_, Dimensiuni dimensiune_, unsigned int passConsecutive_)
            :permiteSuicid(permiteSuicid_), regulaKo(regulaKo_), sfarsitJoc(sfarsitJoc_), komi(komi_), dimensiuneTabla(dimensiune_), passConsecutive(passConsecutive_) {}

    Reguli(const Reguli& other) = default;
    Reguli& operator=(const Reguli& other) = default;
    ~Reguli() = default;

    friend std::ostream& operator<<(std::ostream& os, const Reguli& r);

    bool isMutareValida(const Tabla& tabla, const Mutare& mutare) const;

    friend class Joc;
};

bool Reguli::isMutareValida(const Tabla &tabla, const Mutare &mutare) const {
    if (this->sfarsitJoc) {
        std::cerr<<"Mutare invalida. Sfarsit joc";
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
struct IstoricTabla {
    std::vector<std::vector<Culoare>> stareaTabla;
    bool operator==(const IstoricTabla& other) cont{
        return stareaTabla == other.stareaTabla;
    }
};

class Joc {
private:
    Tabla tabla;
    Jucator jucatorNegru, jucatorAlb;
    Reguli reguli;
    Culoare JucatorCurent;
    unsigned int mutariEfectuate;
    std::vector<IstoricTabla> istoricStari;

    std::set<Pozitie> gasesteGrup(const Pozitie& p) const;
    int verificaCaptura(const Pozitie& p, Culoare culoareInamica);
    void stergeGrup(const std::set<Pozitie>& grup);

public:
    explicit Joc(Dimensiuni dim,
                 const std::string& numeN, Culoare culoareN, tipJ tipN,
                 const std::string& numeA, Culoare culoareA, tipJ tipA,
                 float komi, bool permiteSuicid, bool regulaKo);

    Joc(const Joc& other) = default;
    Joc& operator=(const Joc& other) = default;
    ~Joc() = default;

    friend std::ostream& operator<<(std::ostream& os, const Joc& j);

    bool AplicaMutare(const Mutare& m);
    void CalculeazaScorFinal() const;
    void AfiseazaStareaJocului() const;

    std::set<Pozitie> PublicGasesteGrup(const Pozitie& p) const {
        return gasesteGrup(p);
    }
};

Joc::Joc(Dimensiuni dim,
         const std::string& numeN, Culoare culoareN, tipJ tipN,
         const std::string& numeA, Culoare culoareA, tipJ tipA,
         float komi, bool permiteSuicid, bool regulaKo);
    : tabla(dim),
      jucatorNegru(numeN, 0, culoareN, tipN),
      jucatorAlb(numeA, 0, culoareA, tipA),
      reguli(permiteSuicid, regulaKo, false, komi, dim, o),
      jucatorCurent(Culoare::Negru),
      mutariEfectuate(0){}

std::ostream& operator<<(std::ostream& os, const Joc& j) {
        os << "========= STAREA JOCULUI (" << (j.jucatorCurent == Culoare::Negru ? "Negru" : "Alb" << "la mutare) =========\n";
        os << j.tabla;
        os << j.jucatorNegru;
        os << j.jucatorAlb;
        os << j.reguli;
        os << "Mutari Efectuare: " << j.mutariEfectuare << std::endl;
        os << "==========================================================================\n";
        return os;
    }

void Joc::AfiseazaStareaJocului() const {
        std::cout << *this;
    }

std::set<Pozitie> Joc::gasesteGrup(const Pozitie& p) const {
        std::set<Pozitie> grup;
        std::vector<Pozitie> coada;

        Culoare culoareGrup = tabla.getPozitieCuloare(p);

        if (culoareGrup == Culoare::Gol)
            return grup;

        coada.push_back(p);
        grup.insert(p);

        int n = tabla.getMarime();

        while (!coada.empty()) {
            Pozitie curenta = coada.back();
            coada.pop_back();

            std::vector<Pozitie> vecini = {
                {curenta.x, curenta.y - 1}, {curenta.x, curenta + 1},
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

void Joc::stergeGrup(const std::set<Pozitie>& grup) {
        for (const auto& p : grup)
            tabla.ScoatePiatra(p);
    }

bool Joc::AplicaMutare(const Mutare& m) {
        Culoare culoareCurenta = jucatorCurent;
        Culoare culoareInamica = (jucatorCurent == Culoare::Negru) ? Culoare::Alb : Culoare::Negru;

        if (!Reguli.isMutareValida(tabla, m))
            return false;

        if (m.isPass()) {
            std::cout << (jucatorCurent == Culoare::Negru ? jucatorNegru.nume : jucatorAlb.nume)<< " a pasat.\n";
            reguli.passConsecutive++;
        }
        else {
            Pozitie p = m.getPozitie();
            tabla.Plaseazapiatra(p, culoareCurenta);

            int capturiTotale = 0;
            std::vector<Pozitie> vecini = {
                {p.x, p.y -1}, {p.x, p.y + 1}, {p.x - 1, p.y}, {p.x + 1, p.y}
            };

            for (const auto& vecin : vecini) {
                if (tabla.getPozitieCuloare(vecin) == culoareInamica) {
                    std::set<Pozitie> grupInamic = gasesteGrup(vecin);

                    if (grupInamic.size() > 0 && grupInamic.size() < 4) {
                        capturiTotale += grupInamic.size();
                        stergeGrup(grupInamic);
                        std::cout << "Capturat grup inamic de " << grupInamic.size() << "pietre!\n";
                    }

                }
            }
            if (capturiTotale > 0) {
                if (jucatorCurent == Culoare::Negru)
                    jucatorNegru.AdaugaPietreCapturate(capturiTotale);
                else
                    jucatorAlb.AdaugaPietreCapturate(capturiTotale);
            }

            reguli.passConsecutive = 0;
            mutariEfectuate++;
        }
        jucatorCurent = culoareInamica;
        if (reguli.passConsecutive >= 2) {
            reguli.sfarsitJoc = true;
            std::cout << "Jocul s-a incheiat prin 2 Pass-uri consecutive!\n";
        }

        return true;
    }

void Joc::CalculeazaScorFinal() const {
        float scorNegru = (float) jucatorNegru.pietreCapturate;
        float scorAlb = (float) jucatorAlb.pietreCapturate + reguli.komi;

        float teritoriuNegru = 0.0f;
        float teritoriuAlb = 0.0f;

        scorNegru = scorNegru + teritoriuNegru;
        scorAlb = scorAlb + teritoriuAlb;

        std::cout << "\n============== CALCU SCOR FINAL =============\n";
        std::cout << "Capturi Negru: "<< jucatorNegru.pietreCapturate << "(+ Teritoriu : " << teritoriuNegru << ")\n";
        std::cout << "Capturi Alb: "<< jucatorAlb.pietreCapturate << " (+ Komi: " << reguli.komi << " + Teritoriu: " << teritoriuAlb << ")\n";

        std::cout<< "\n** SCOR FINAL NEGRU: " << scorNegru << "**\n";
        std::cout << "\n** SCOR FINAL ALB: " << scorAlb << "**\n";

        if (scorNegru > scorAlb)
            std::cout << "CASTIGA JUCATORUL NEGRU!\n";
        else if (scorNegru < scorAlb)
            std::cout << "CASTIGA JUCATORUL ALB!\n";
        else
            std::cout << "REMIZA (JIGO)\n";

        std::cout <<"==============================================\n";
    }

int main() {
    // to be continued
    return 0;
}
