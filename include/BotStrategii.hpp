#ifndef BOTSTRATEGII_HPP
#define BOTSTRATEGII_HPP

#include <cstddef>
#include <memory>
#include <unordered_set>

#include "Culoare.hpp"
#include "Mutare.hpp"

class Joc;
class Tabla;

// Nivelele de dificultate disponibile pentru bot.
enum class NivelBot { Usor, Mediu, Avansat };

// Interfata strategiei de joc a botului (Design Pattern Strategy).
// Fiecare nivel concrect implementeaza propria politica de alegere
// a mutarilor, dar garanteaza INTOTDEAUNA o mutare valida.
class IBotStrategy {
public:
    virtual ~IBotStrategy() = default;

    [[nodiscard]] virtual std::unique_ptr<IBotStrategy> clone() const = 0;

    // Alege si returneaza o mutare legala (plasare sau pass) pentru
    // jucatorul aflat la rand in contextul 'joc'.
    virtual Mutare alegeMutare(const Joc& joc) const = 0;

    [[nodiscard]] virtual const char* numeStrategie() const = 0;
};

// Usoar: alege aleatoriu dintre TOATE mutarile valide (fara incercari oarbe).
class BotUsor : public IBotStrategy {
public:
    [[nodiscard]] std::unique_ptr<IBotStrategy> clone() const override;
    Mutare alegeMutare(const Joc& joc) const override;
    [[nodiscard]] const char* numeStrategie() const override { return "Usor"; }
};

// Mediu: euristici simple pe baza de reguli:
//  1. captureaza grupurile adverse adiacente;
//  2. salveaza grupurile proprii aflate in Atari (1 singura libertate);
//  3. joaca langa piesa adversarului.
class BotMediu : public IBotStrategy {
public:
    [[nodiscard]] std::unique_ptr<IBotStrategy> clone() const override;
    Mutare alegeMutare(const Joc& joc) const override;
    [[nodiscard]] const char* numeStrategie() const override { return "Mediu"; }
};

// Avansat: cautare Minimax cu taiere alfa-beta (adancime configurabila)
// si evaluare pe teritoriu + pietre + capturi.
class BotAvansat : public IBotStrategy {
public:
    explicit BotAvansat(unsigned int adancime = 2);

    [[nodiscard]] std::unique_ptr<IBotStrategy> clone() const override;
    Mutare alegeMutare(const Joc& joc) const override;
    [[nodiscard]] const char* numeStrategie() const override { return "Avansat"; }

private:
    unsigned int adancime_;
};

// Factory: creeaza strategia corespunzatoare nivelului ales.
std::unique_ptr<IBotStrategy> creeazaStrategie(NivelBot nivel);

#endif
