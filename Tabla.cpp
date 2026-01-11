#include "Tabla.hpp"

#ifndef GO_HEADLESS
#include <SFML/Graphics.hpp>
#endif

// initializarea tablei
Tabla::Tabla(Dimensiuni d)
    : marime(static_cast<unsigned int>(d)) {
    grila.resize(marime, std::vector<Culoare>(marime, Culoare::Gol));
}

#ifndef GO_HEADLESS
// randare grafica
void Tabla::draw(sf::RenderWindow& window) const {

    // ================== LAYOUT INTERN ==================
    // dimensiunea maxima a tablei (fixa, coerenta)
    constexpr float boardMaxSize = 600.f;

    // distanta intre linii (IMPORTANT: marime - 1)
    const float cellSize = boardMaxSize / static_cast<float>(marime - 1);
    const float boardSize = cellSize * static_cast<float>(marime - 1);

    // offseturi (centrare verticala)
    constexpr float offsetX = 50.f;
    const float offsetY =
        (static_cast<float>(window.getSize().y) - boardSize) / 2.f;

    const float stoneRadius = cellSize * 0.45f;
    // ===================================================

    sf::Color culoareGrid = sf::Color::White;

    // desen grid
    for (unsigned int i = 0; i < marime; ++i) {

        sf::Vertex hLine[] = {
            sf::Vertex(
                {offsetX, offsetY + static_cast<float>(i) * cellSize},
                culoareGrid
            ),
            sf::Vertex(
                {offsetX + static_cast<float>(marime - 1) * cellSize,
                 offsetY + static_cast<float>(i) * cellSize},
                culoareGrid
            )
        };
        window.draw(hLine, 2, sf::Lines);

        sf::Vertex vLine[] = {
            sf::Vertex(
                {offsetX + static_cast<float>(i) * cellSize, offsetY},
                culoareGrid
            ),
            sf::Vertex(
                {offsetX + static_cast<float>(i) * cellSize,
                 offsetY + static_cast<float>(marime - 1) * cellSize},
                culoareGrid
            )
        };
        window.draw(vLine, 2, sf::Lines);
    }

    // desen pietre
    sf::CircleShape piesa(stoneRadius);
    piesa.setOrigin(stoneRadius, stoneRadius);

    for (unsigned int i = 0; i < marime; ++i) {
        for (unsigned int j = 0; j < marime; ++j) {

            if (grila[i][j] != Culoare::Gol) {
                piesa.setPosition(
                    offsetX + static_cast<float>(j) * cellSize,
                    offsetY + static_cast<float>(i) * cellSize
                );

                piesa.setFillColor(
                    grila[i][j] == Culoare::Negru
                        ? sf::Color::Black
                        : sf::Color::White
                );

                piesa.setOutlineThickness(1.5f);
                piesa.setOutlineColor(
                    sf::Color(100, 100, 100)
                );

                window.draw(piesa);
            }
        }
    }
}
#endif

bool Tabla::esteGol(Pozitie p) const {
    return p.x < marime &&
           p.y < marime &&
           grila[p.x][p.y] == Culoare::Gol;
}

void Tabla::Plaseazapiatra(Pozitie p, Culoare c) {
    if (p.x < marime && p.y < marime) {
        grila[p.x][p.y] = c;
    }
}

Culoare Tabla::getPozitieCuloare(Pozitie p) const {
    return (p.x < marime && p.y < marime)
               ? grila[p.x][p.y]
               : Culoare::Gol;
}

unsigned int Tabla::getMarime() const {
    return marime;
}

const std::vector<std::vector<Culoare>>&
Tabla::getGrila() const {
    return grila;
}

void Tabla::setGrila(
    const std::vector<std::vector<Culoare>>& g
) {
    grila = g;
}
