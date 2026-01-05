#ifndef JUCATOR_HPP
#define JUCATOR_HPP

#include <string>
#include <iostream>

#include "Pozitie.hpp"

enum class Culoare;
class Tabla;
class Mutare;

class Jucator {
protected:
    std::string nume;
    Culoare culoare;
    unsigned int pietreCapturate;
public:
    Jucator(std::string nume_, Culoare culoare_);
    virtual ~Jucator() = default;

    virtual Jucator* clone() const = 0;

    virtual Mutare alegeMutare(const Tabla& t) = 0;

    std::string getNume() const { return nume; }
    Culoare getCuloare() const { return culoare; }
    unsigned int getCapturi() const { return pietreCapturate; }
    void adaugaCapturi(unsigned int nr) {pietreCapturate += nr; }

};

class JucatorUman : public Jucator {
public:
    using Jucator::Jucator;
    Jucator* clone() const override {return new JucatorUman(*this); }
    Mutare alegeMutare(const Tabla& t) override;
};

class JucatorBot : public Jucator {
public:
    using Jucator::Jucator;
    Jucator* clone() const override {return new JucatorBot(*this); }
    Mutare alegeMutare(const Tabla& t) override;
};

#endif
