#ifndef TABLA_HPP
#define TABLA_HPP

#include <vector>
#include "Culoare.hpp"
#include "Pozitie.hpp"

#ifndef CH_CI_RUN
    #include <SFML/Graphics.hpp>
#else
    // Pe CI, unde nu avem SFML Graphics, declaram doar ca exista clasa RenderWindow
    namespace sf { class RenderWindow; }
#endif

class Tabla {
private:
    std::vector<std::vector<Culoare>> grila;
    unsigned int marime; // dimensiune tabla

public:
    explicit Tabla(Dimensiuni d);
    //tabla de joc si pietrele
    void draw(sf::RenderWindow& window) const;

    [[nodiscard]] bool esteGol(Pozitie p) const;

    [[nodiscard]] Culoare getPozitieCuloare(Pozitie p) const;

    [[nodiscard]] unsigned int getMarime() const;


    [[nodiscard]] const std::vector<std::vector<Culoare>>& getGrila() const;

    void Plaseazapiatra(Pozitie p, Culoare c);

    void setGrila(const std::vector<std::vector<Culoare>>& g);

    friend std::ostream& operator<<(std::ostream& os, const Tabla& t);
};

#endif