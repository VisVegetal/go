#include <vector>
#include <array>
#include <utility>
#include <string>
#include <fstream>
#include <set>
#include <iostream>


enum class Culoare{Negru, Alb, **Gol**};
enum class Dimensiuni{D9x9, D13x13, D19x19};
enum class tipJ{uman, AI};
enum class tipM{plasare, pass};

struct Pozitie {
    unsigned int x, y;
    bool operator<(const Pozitie& other) const {
        return std::tie(x, y) < std::tie(other.x, other.y);
    }
};

std::ostream& operator<<(std::ostream& os, const Pozitie& p) {
    os << "(" << p.x << ", " << p.y << ")";
    return os;
}



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
    std::string culoare_str = (p.culoare == Culoare::Negru ? "Negru" : "Alb");
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
        bool esteGol(const Pozitie& p) const;
};

bool Tabla::esteGol(const Pozitie& p) const {
    unsigned int n = getMarime();
    if (p.x < n && p.y < n)
        return tabla[p.x][p.y] == Culoare::Gol;
    return false;
}

void Tabla::Plaseazapiatra(const Pozitie &p, Culoare c) {
    unsigned int n = getMarime();

    if (p.x < n && p.y < n) {
        if(esteGol(p)) {
            this->tabla[p.x][p.y] = c;
            std::cout << "Piatra plasata cu succes: " << (c == Culoare::Negru ? "Negru" : "Alb")<< " la pozitia "<< p << ".\n";

        }
        else {
            std::cerr<< "Eroare: Pozitia "<< p << "este in afara limitelor (" << n<< "x"<<").\n";
        }

    }
}

std::ostream& operator<<(std::ostream& os, const Tabla& t) {
    unsigned int n = t.getMarime();

    os<<"---Tabla de GO(" << n << "x" << n << ")---\n";
    os << " ";
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
};

void Jucator::AdaugaPietreCapturate(unsigned int nr) {
    if (nr > 0) {
        this->pietreCapturate += nr;
        std::cout<<this->nume<<"a capturat"<< nr << "pietr. Total pietre capturate: "<< this->pietreCapturate<<".\n";
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
        std::cout<<"Libertate adaugata la: "<< p << "pentru grupul: "<< id<< ".\n";
    else
        std::cerr<<"Libertatea "<<p<<"exista deja in Grupul "<< id << ".\n";
}
std::ostream& operator<<(std::ostream& os, const Grup& grup) {
    os<<"---Grup ID: "<<grup.id<<"---\n"
      <<"Culoare: "<<(grup.culoare == Culoare::Negru ? "Negru" : "Alb")<<"\n"
      <<"Pietre: "<< grup.pozitiiPietre.size()<<" (" << (g.capturat ? "CAPTURAT" : "Activ") << ")\n"
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
};

bool Reguli::isMutareValida(const Tabla &tabla, const Mutare &mutare) const {
    if (this->sfarsitJoc) {
        std::cerr<<"Mutare invalida. Sfarsit joc";
        return false;
    }
    if (mutare.isPass())
        return true;

    Pozitie p = mutare.pozitie;

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


int main() {
    //nu i terminat
    return 0;
}
