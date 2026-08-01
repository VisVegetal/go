#include "../include/Jucator.hpp"
#include "../include/Joc.hpp"

Mutare JucatorUman::alegeMutare(const Joc&) const {
    // Mutarile jucatorului uman provin din interfata grafica.
    return Mutare({0, 0}, tipM::pass);
}

JucatorBot::JucatorBot(std::string n, Culoare c, std::unique_ptr<IBotStrategy> strat)
    : Jucator(std::move(n), c), strategie(std::move(strat)) {}

JucatorBot::JucatorBot(const JucatorBot& altul)
    : Jucator(altul),
      strategie(altul.strategie ? altul.strategie->clone() : nullptr) {}

JucatorBot& JucatorBot::operator=(const JucatorBot& altul) {
    if (this == &altul) {
        return *this;
    }
    Jucator::operator=(altul);
    strategie = altul.strategie ? altul.strategie->clone() : nullptr;
    return *this;
}

std::unique_ptr<Jucator> JucatorBot::clone() const {
    return std::make_unique<JucatorBot>(*this);
}

Mutare JucatorBot::alegeMutare(const Joc& joc) const {
    return strategie->alegeMutare(joc);
}

const char* JucatorBot::getNumeStrategie() const {
    return strategie ? strategie->numeStrategie() : "?";
}
