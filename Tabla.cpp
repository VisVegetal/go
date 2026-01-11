#include "Tabla.hpp"

//initializarea tablei
Tabla::Tabla(Dimensiuni d) : marime(static_cast<unsigned int>(d)) {
    grila.resize(marime, std::vector<Culoare>(marime, Culoare::Gol));
}

//randare grafica
void Tabla::draw(sf::RenderWindow& window) const {
#ifndef CH_CI_RUN
    constexpr float cellSize = 40.0f;
    constexpr float offset = 50.0f;
    constexpr float stoneRadius = 18.0f;
    sf::Color culoareGrid = sf::Color::White;

    //desen grid
    for (unsigned int i = 0; i < marime; ++i) {
        sf::Vertex hLine[] = {
            sf::Vertex(sf::Vector2f(offset, offset + static_cast<float>(i) * cellSize), culoareGrid),
            sf::Vertex(sf::Vector2f(offset + static_cast<float>(marime - 1) * cellSize, offset + static_cast<float>(i) * cellSize), culoareGrid)
        };
        window.draw(hLine, 2, sf::Lines);

        sf::Vertex vLine[] = {
            sf::Vertex(sf::Vector2f(offset + static_cast<float>(i) * cellSize, offset), culoareGrid),
            sf::Vertex(sf::Vector2f(offset + static_cast<float>(i) * cellSize, offset + static_cast<float>(marime - 1) * cellSize), culoareGrid)
        };
        window.draw(vLine, 2, sf::Lines);
    }

    //desen pietre
    sf::CircleShape piesa(stoneRadius);
    piesa.setOrigin(stoneRadius, stoneRadius);
    for (unsigned int i = 0; i < marime; ++i) {
        for (unsigned int j = 0; j < marime; ++j) {
            if (grila[i][j] != Culoare::Gol) {
                piesa.setPosition(offset + static_cast<float>(j) * cellSize, offset + static_cast<float>(i) * cellSize);
                piesa.setFillColor(grila[i][j] == Culoare::Negru ? sf::Color::Black : sf::Color::White);
                piesa.setOutlineThickness(1.5f);
                piesa.setOutlineColor(sf::Color(100, 100, 100));
                window.draw(piesa);
            }
        }
    }
#else
    if (&window == nullptr) return;
#endif
}

bool Tabla::esteGol(Pozitie p) const {
    return p.x < marime && p.y < marime && grila[p.x][p.y] == Culoare::Gol;
}
void Tabla::Plaseazapiatra(Pozitie p, Culoare c) {
    if (p.x < marime && p.y < marime) grila[p.x][p.y] = c;
}
Culoare Tabla::getPozitieCuloare(Pozitie p) const {
    return (p.x < marime && p.y < marime) ? grila[p.x][p.y] : Culoare::Gol;
}
unsigned int Tabla::getMarime() const {
    return marime;
}
const std::vector<std::vector<Culoare>>& Tabla::getGrila() const {
    return grila;
}
void Tabla::setGrila(const std::vector<std::vector<Culoare>>& g) {
    grila = g;
}