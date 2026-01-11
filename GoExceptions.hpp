#ifndef GOEXCEPTIONS_HPP
#define GOEXCEPTIONS_HPP

#include <exception>
#include <string>

class GoException : public std::exception {
protected:
    std::string mesaj;
public:

    explicit GoException(std::string m) : mesaj(std::move(m)) {}

    [[nodiscard]] const char* what() const noexcept override {
        return mesaj.c_str();
    }

};

class CoordonateInvalideException : public GoException {
public:
    CoordonateInvalideException(int x, int y)
        : GoException("Eroare: Coordonatele (" + std::to_string(x) + "," + std::to_string(y) + ") sunt in afara tablei!") {}
};

class MutareIlegalaException : public GoException {
public:
    explicit MutareIlegalaException(const std::string& detaliu)
        : GoException("Mutare Ilegala: " + detaliu) {}
};

#endif

