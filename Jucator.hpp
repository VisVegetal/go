#ifndef JUCATOR_HPP
#define JUCATOR_HPP

#include <string>
#include "Culoare.hpp"
#include "Mutare.hpp"

class Tabla;

//clasa de baza abstracta
class Jucator {
protected:
    std::string nume;
    Culoare culoare;
public:
    Jucator(std::string n, Culoare c) : nume(std::move(n)), culoare(c) {}
    virtual ~Jucator() = default;
    [[nodiscard]] virtual Jucator* clone() const = 0;
    virtual Mutare alegeMutare(const Tabla& t) = 0;
    [[maybe_unused]][[nodiscard]] const std::string& getNume() const { return nume; }
};

class JucatorUman : public Jucator {
public:
    using Jucator::Jucator;
    [[nodiscard]] Jucator* clone() const override { return new JucatorUman(*this); }
    Mutare alegeMutare(const Tabla& t) override;
};

class JucatorBot : public Jucator {
public:
    using Jucator::Jucator;
    [[nodiscard]] Jucator* clone() const override { return new JucatorBot(*this); }
    Mutare alegeMutare(const Tabla& t) override;
};

#endif