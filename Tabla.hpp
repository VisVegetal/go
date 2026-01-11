#ifndef TABLA_HPP
#define TABLA_HPP

#include <vector>
#include "Culoare.hpp"
#include "Pozitie.hpp"

#if !defined(GO_HEADLESS) && !defined(NO_GRAPHICS)
#include <SFML/Graphics.hpp>
#endif

class Tabla {
private:
    std::vector<std::vector<Culoare>> grila;
    unsigned int marime; // dimensiune tabla

public:
    explicit Tabla(Dimensiuni d);

#if !defined(GO_HEADLESS) && !defined(NO_GRAPHICS)
    //tabla de joc si pietrele
    void draw(sf::RenderWindow& window) const;
#endif

    [[nodiscard]] bool esteGol(Pozitie p) const;
    [[nodiscard]] Culoare getPozitieCuloare(Pozitie p) const;
    [[nodiscard]] unsigned int getMarime() const;
    [[nodiscard]] const std::vector<std::vector<Culoare>>& getGrila() const;

    void Plaseazapiatra(Pozitie p, Culoare c);
    void setGrila(const std::vector<std::vector<Culoare>>& g);

    friend std::ostream& operator<<(std::ostream& os, const Tabla& t);
};

#endif
