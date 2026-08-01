#include "../include/Tabla.hpp"

#ifndef GO_HEADLESS
#include <SFML/Graphics.hpp>
#endif

#include <utility>
#include <vector>

// initializarea tablei
Tabla::Tabla(Dimensiuni d)
    : marime(static_cast<unsigned int>(d)) {
    grila.resize(marime, std::vector<Culoare>(marime, Culoare::Gol));
}

#if !defined(GO_HEADLESS) && !defined(NO_GRAPHICS)
// randare grafica: tabla de lemn, grila, puncte star, pietre realiste
// si piatra fantoma (hover).
void Tabla::draw(sf::RenderWindow& window,
                 const std::optional<GhostStone>& fantoma) const {

    // layout intern: dimensiunea maxima a tablei (fixa, coerenta)
    constexpr float boardMaxSize = 600.f;

    // distanta intre linii (marime - 1)
    const float cellSize = boardMaxSize / static_cast<float>(marime - 1);
    const float boardSize = cellSize * static_cast<float>(marime - 1);

    // offseturi (centrare verticala)
    constexpr float offsetX = 50.f;
    const float offsetY =
        (static_cast<float>(window.getSize().y) - boardSize) / 2.f;

    const float stoneRadius = cellSize * 0.45f;
    constexpr float padding = 18.f;

    // Fundal din lemn cald (#DCB35C), cu margine inchisa la culoare.
    sf::RectangleShape fundal({
        boardSize + 2.f * (stoneRadius + padding),
        boardSize + 2.f * (stoneRadius + padding)
    });
    fundal.setPosition(offsetX - stoneRadius - padding,
                       offsetY - stoneRadius - padding);
    fundal.setFillColor(sf::Color(0xDC, 0xB3, 0x5C));
    fundal.setOutlineThickness(3.f);
    fundal.setOutlineColor(sf::Color(0x8A, 0x6B, 0x3A));
    window.draw(fundal);

    // Linii de grila: distincte, dar nu stridente (maro inchis).
    const sf::Color culoareGrid(0x6B, 0x4A, 0x23);
    for (unsigned int i = 0; i < marime; ++i) {

        sf::Vertex hLine[] = {
            sf::Vertex(
                {offsetX, offsetY + static_cast<float>(i) * cellSize},
                culoareGrid
            ),
            sf::Vertex(
                {offsetX + boardSize, offsetY + static_cast<float>(i) * cellSize},
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
                {offsetX + static_cast<float>(i) * cellSize, offsetY + boardSize},
                culoareGrid
            )
        };
        window.draw(vLine, 2, sf::Lines);
    }

    // Puncte de star (hoshi), specific tablei traditionale de Go.
    std::vector<std::pair<unsigned int, unsigned int>> hoshi;
    if (marime == 19) {
        for (unsigned int r : {3u, 9u, 15u})
            for (unsigned int c : {3u, 9u, 15u})
                hoshi.emplace_back(r, c);
    } else if (marime == 13) {
        for (unsigned int r : {3u, 6u, 9u})
            for (unsigned int c : {3u, 6u, 9u})
                hoshi.emplace_back(r, c);
    } else {
        for (unsigned int r : {2u, 4u, 6u})
            for (unsigned int c : {2u, 4u, 6u})
                hoshi.emplace_back(r, c);
    }

    sf::CircleShape punct(stoneRadius * 0.18f);
    punct.setOrigin(stoneRadius * 0.18f, stoneRadius * 0.18f);
    punct.setFillColor(sf::Color(0x4E, 0x36, 0x18));
    for (const auto& [r, c] : hoshi) {
        punct.setPosition(offsetX + static_cast<float>(c) * cellSize,
                          offsetY + static_cast<float>(r) * cellSize);
        window.draw(punct);
    }

    // Pietre: contur subtil + reflexie, pentru un aspect de piatra reala.
    for (unsigned int i = 0; i < marime; ++i) {
        for (unsigned int j = 0; j < marime; ++j) {

            if (grila[i][j] == Culoare::Gol) {
                continue;
            }

            const auto centru = sf::Vector2f(
                offsetX + static_cast<float>(j) * cellSize,
                offsetY + static_cast<float>(i) * cellSize
            );

            const bool neagra = (grila[i][j] == Culoare::Negru);

            sf::CircleShape piesa(stoneRadius);
            piesa.setOrigin(stoneRadius, stoneRadius);
            piesa.setPosition(centru);
            piesa.setFillColor(neagra ? sf::Color(20, 20, 24)
                                      : sf::Color(240, 240, 238));
            piesa.setOutlineThickness(neagra ? 1.6f : 1.2f);
            piesa.setOutlineColor(neagra ? sf::Color(150, 150, 158)
                                         : sf::Color(120, 120, 120));
            window.draw(piesa);

            // Reflexie subtila, in coltul stanga-sus.
            sf::CircleShape reflexie(stoneRadius * 0.32f);
            reflexie.setOrigin(stoneRadius * 0.32f, stoneRadius * 0.32f);
            reflexie.setPosition(
                centru - sf::Vector2f(stoneRadius * 0.38f,
                                      stoneRadius * 0.38f));
            reflexie.setFillColor(
                sf::Color(255, 255, 255, neagra ? 45 : 150));
            window.draw(reflexie);
        }
    }

    // Piatra fantoma (hover): semitransparenta, arata mutarea iminenta.
    if (fantoma.has_value()) {
        sf::CircleShape fant(stoneRadius);
        fant.setOrigin(stoneRadius, stoneRadius);
        fant.setPosition(fantoma->pozitieEcran);
        fant.setFillColor(fantoma->culoare == Culoare::Negru
                              ? sf::Color(20, 20, 24, 120)
                              : sf::Color(240, 240, 238, 120));
        fant.setOutlineThickness(1.2f);
        fant.setOutlineColor(fantoma->culoare == Culoare::Negru
                                 ? sf::Color(150, 150, 158, 160)
                                 : sf::Color(120, 120, 120, 160));
        window.draw(fant);
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

void Tabla::eliminaPiatra(Pozitie p) {
    if (p.x < marime && p.y < marime) {
        grila[p.x][p.y] = Culoare::Gol;
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
