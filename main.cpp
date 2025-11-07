#include <utility>
#include <vector>
#include <string>
#include <fstream>
#include <set>
#include <iostream>
#include <tuple>

//declarare pentru vizibilitate in clasele joc si reguli
void TestareJoc(std::istream& is);

enum class Culoare{Negru, Alb, Gol};
enum class Dimensiuni{D9x9, D13x13, D19x19};
enum class tipJ{uman};
enum class tipM{plasare, pass}; // tipurile de mutari posibile
//coordonatele x si y

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

    Pietre(const Pietre& other) = default;
    Pietre& operator=(const Pietre& other) = default;
    ~Pietre() = default;

    //schimbarea culorii, rar folosita in GO
    void SchimbaCuloarea() {
        if (this->culoare == Culoare::Negru)
            this->culoare = Culoare::Alb;
        if (this->culoare == Culoare::Alb)
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

    Tabla(const Tabla& other) = default;
    Tabla& operator=(const Tabla& other) = default;
    ~Tabla() = default;

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

    Jucator(const Jucator& other) = default;
    Jucator& operator=(const Jucator& other) = default;
    ~Jucator() = default;

    friend std::ostream& operator<<(std::ostream& os, const Jucator& j);

    void AdaugaPietreCapturate(unsigned int nr);

    [[nodiscard]] std::string getNume() const { return nume; }

    [[nodiscard]] tipJ getTipJucator() const { return tip_jucator; }

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

    Mutare(const Mutare& other) = default;
    Mutare& operator=(const Mutare& other) = default;
    ~Mutare() = default;

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

//clasa geup
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

    Grup(const Grup& other) = default;
    Grup& operator=(const Grup& other) = default;
    ~Grup() = default;

    friend std::ostream& operator<<(std::ostream& os, const Grup& grup);

    void AdaugaLibertate(const Pozitie& p);
};

//incearca sa adauge o noua pozitie goala (libertate) la grup
void Grup::AdaugaLibertate(const Pozitie& p) {
    if (this->libertati.insert(p).second)
        std::cout << "Libertate adaugata la: " << p << " pentru grupul: " << id << ".\n";
    else
        std::cerr << "Libertatea " << p << " exista deja in Grupul " << id << ".\n";
}

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
