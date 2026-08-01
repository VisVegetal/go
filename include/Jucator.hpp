#ifndef JUCATOR_HPP
#define JUCATOR_HPP

#include <memory>
#include <string>
#include <utility>

#include "BotStrategii.hpp"
#include "Culoare.hpp"
#include "Mutare.hpp"

class Joc;
class Tabla;

// Clasa de baza abstracta a unui jucator (uman sau bot).
class Jucator {
protected:
    std::string nume;
    Culoare culoare;

public:
    Jucator(std::string n, Culoare c) : nume(std::move(n)), culoare(c) {}

    virtual ~Jucator() = default;

    [[nodiscard]] virtual std::unique_ptr<Jucator> clone() const = 0;

    // Alege o mutare in contextul partidei curente.
    virtual Mutare alegeMutare(const Joc& joc) const = 0;

    [[nodiscard]] const std::string& getNume() const { return nume; }
    [[nodiscard]] Culoare getCuloare() const { return culoare; }
};

// Jucatorul uman: mutarile vin din interfata grafica; aici doar declaram un pass.
class JucatorUman : public Jucator {
public:
    using Jucator::Jucator;

    [[nodiscard]] std::unique_ptr<Jucator> clone() const override {
        return std::make_unique<JucatorUman>(*this);
    }

    Mutare alegeMutare(const Joc&) const override;
};

// Bot controlat de o strategie concreta (Design Pattern Strategy).
class JucatorBot : public Jucator {
private:
    std::unique_ptr<IBotStrategy> strategie;

public:
    JucatorBot(std::string n, Culoare c, std::unique_ptr<IBotStrategy> strat);

    // Reguli de copiere: strategia este clonata (deep copy).
    JucatorBot(const JucatorBot& altul);
    JucatorBot& operator=(const JucatorBot& altul);
    JucatorBot(JucatorBot&&) noexcept = default;
    JucatorBot& operator=(JucatorBot&&) noexcept = default;

    [[nodiscard]] std::unique_ptr<Jucator> clone() const override;
    Mutare alegeMutare(const Joc& joc) const override;

    [[nodiscard]] const char* getNumeStrategie() const;
};

#endif
