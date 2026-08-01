#ifndef GOEXCEPTIONS_HPP
#define GOEXCEPTIONS_HPP

#include <exception>
#include <string>
#include <utility>

// Clasa de baza pentru toate erorile specifice jocului de Go.
// Fiecare situatie ilegala are propriul tip derivat, astfel incat
// interfata grafica poate afisa mesaje sugestive utilizatorului.
class GoException : public std::exception {
protected:
    std::string mesaj;

public:
    explicit GoException(std::string m) : mesaj(std::move(m)) {}

    [[nodiscard]] const char* what() const noexcept override {
        return mesaj.c_str();
    }
};

// Coordonatele date depasesc limitele tablei.
class CoordonateInvalideException : public GoException {
public:
    CoordonateInvalideException(unsigned int x, unsigned int y)
        : GoException("Coordonatele (" + std::to_string(x) + "," + std::to_string(y) + ") sunt in afara tablei!") {}
};

// Mutarea nu respecta regulile jocului (eroare generica de mutare invalida).
class MutareInvalidaException : public GoException {
public:
    explicit MutareInvalidaException(const std::string& detaliu)
        : GoException("Mutare invalida: " + detaliu) {}
};

// Pozitia aleasa este deja ocupata de o piesa.
class PozitieOcupataException : public GoException {
public:
    explicit PozitieOcupataException(const std::string& detaliu)
        : GoException("Pozitie ocupata: " + detaliu) {}
};

// Mutarea ar repeta o stare anterioara a tablei (incalcarea regulii Ko).
class RegulaKoException : public GoException {
public:
    explicit RegulaKoException(const std::string& detaliu)
        : GoException("Regula Ko: " + detaliu) {}
};

// Plasarea piesel ar lasa propriul grup fara libertati (interdictia sinuciderii).
class MutareSuicidalaException : public GoException {
public:
    explicit MutareSuicidalaException(const std::string& detaliu)
        : GoException("Mutare suicidala: " + detaliu) {}
};

// Jocul s-a incheiat (2 pass-uri consecutive sau cedare); nu se mai pot face mutari.
class JocTerminatException : public GoException {
public:
    JocTerminatException()
        : GoException("Jocul s-a terminat! Nu se mai pot efectua mutari.") {}
};

#endif
