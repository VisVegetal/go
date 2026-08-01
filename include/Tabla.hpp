#ifndef TABLA_HPP
#define TABLA_HPP

#include <optional>
#include <vector>
#include "Culoare.hpp"
#include "Pozitie.hpp"

#if !defined(GO_HEADLESS) && !defined(NO_GRAPHICS)
#include <SFML/Graphics.hpp>

// Piatra fantoma (preview de hover): arata unde si in ce culoare
// s-ar aseza urmatoarea piatra pe tabla.
struct GhostStone {
    sf::Vector2f pozitieEcran;
    Culoare culoare;
};
#endif

class Tabla {
private:
    std::vector<std::vector<Culoare>> grila;
    unsigned int marime; // dimensiune tabla

public:
    explicit Tabla(Dimensiuni d);

#if !defined(GO_HEADLESS) && !defined(NO_GRAPHICS)
    void draw(sf::RenderWindow& window,
              const std::optional<GhostStone>& fantoma = std::nullopt) const;
#endif

    [[nodiscard]] bool esteGol(Pozitie p) const;
    [[nodiscard]] Culoare getPozitieCuloare(Pozitie p) const;
    [[nodiscard]] unsigned int getMarime() const;
    [[nodiscard]] const std::vector<std::vector<Culoare>>& getGrila() const;

    void Plaseazapiatra(Pozitie p, Culoare c);
    void eliminaPiatra(Pozitie p);
    void setGrila(const std::vector<std::vector<Culoare>>& g);
};

#endif
