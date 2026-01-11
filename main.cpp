#include <iostream>
#include <sstream>
#include <cmath>
#include <memory>

#include "Joc.hpp"
#include "GoExceptions.hpp"

#ifndef GO_HEADLESS
#include <SFML/Graphics.hpp>
#endif

#if defined(CH_CI_RUN) || defined(GO_HEADLESS)
static int ruleazaSmokeTestHeadless() {
    auto j1 = std::make_unique<JucatorUman>("Jucator", Culoare::Negru);
    auto j2 = std::make_unique<JucatorBot>("Bot", Culoare::Alb);

    Joc partida(Dimensiuni::D9x9, j1.get(), j2.get());

    auto castigator = partida.determinaCastigator();
    auto sugestie   = partida.sugereazaMutare();

    if (!castigator.empty()) {
        std::cout << castigator << "\n";
    }

    if (sugestie.has_value()) {
        std::cout << sugestie->x << "," << sugestie->y << "\n";
    }


    return 0;
}
#endif

#ifndef GO_HEADLESS
[[maybe_unused]]static Dimensiuni alegeDimensiuneGUI(sf::Font& font) {
    sf::RenderWindow menu(sf::VideoMode(520, 320), "Alege dimensiunea tablei");
    menu.setFramerateLimit(60);

    sf::Text title("Alege dimensiunea tablei:", font, 26);
    title.setPosition(40.f, 30.f);

    auto makeBtn = [&](float x, const std::string& t) {
        sf::RectangleShape b({140.f, 60.f});
        b.setPosition(x, 140.f);
        b.setFillColor(sf::Color(70,70,70));
        b.setOutlineThickness(2);
        b.setOutlineColor(sf::Color::White);

        sf::Text txt(t, font, 22);
        txt.setPosition(x + 30.f, 155.f);
        return std::pair{b, txt};
    };

    auto b9  = makeBtn(40.f,  "9 x 9");
    auto b13 = makeBtn(190.f, "13 x 13");
    auto b19 = makeBtn(340.f, "19 x 19");

    while (menu.isOpen()) {
        sf::Event e{};
        while (menu.pollEvent(e)) {
            if (e.type == sf::Event::Closed)
                menu.close();

            if (e.type == sf::Event::MouseButtonPressed) {
                sf::Vector2f m(
                    static_cast<float>(e.mouseButton.x),
                    static_cast<float>(e.mouseButton.y)
                );
                if (b9.first.getGlobalBounds().contains(m))  return Dimensiuni::D9x9;
                if (b13.first.getGlobalBounds().contains(m)) return Dimensiuni::D13x13;
                if (b19.first.getGlobalBounds().contains(m)) return Dimensiuni::D19x19;
            }
        }

        menu.clear(sf::Color(30,30,30));
        menu.draw(title);
        menu.draw(b9.first);  menu.draw(b9.second);
        menu.draw(b13.first); menu.draw(b13.second);
        menu.draw(b19.first); menu.draw(b19.second);
        menu.display();
    }

    return Dimensiuni::D9x9;
}
#endif

int main() {

#if defined(CH_CI_RUN) || defined(GO_HEADLESS)
    return ruleazaSmokeTestHeadless();
#else

    sf::Font font;
    if (!font.loadFromFile("arial.ttf")) {
        std::cerr << "Eroare: font lipsa\n";
        return -1;
    }

    Dimensiuni dim = alegeDimensiuneGUI(font);
    const auto N = static_cast<unsigned int>(dim);

    sf::RenderWindow window(sf::VideoMode(1100, 800), "Go - Proiect POO");
    window.setFramerateLimit(60);

    auto j1 = std::make_unique<JucatorUman>("Jucator", Culoare::Negru);
    auto j2 = std::make_unique<JucatorBot>("AlphaGo", Culoare::Alb);

    Joc partida(dim, j1.get(), j2.get());

    // butoane
    sf::RectangleShape exitBtn({140.f, 50.f});
    exitBtn.setPosition(750.f, 50.f);
    exitBtn.setFillColor(sf::Color(120, 20, 20));
    sf::Text exitTxt("EXIT", font, 24);
    exitTxt.setPosition(790.f, 60.f);

    sf::RectangleShape passBtn({140.f, 50.f});
    passBtn.setPosition(750.f, 120.f);
    passBtn.setFillColor(sf::Color::Red);
    sf::Text passTxt("PASS", font, 24);
    passTxt.setPosition(795.f, 130.f);

    sf::RectangleShape undoBtn({140.f, 50.f});
    undoBtn.setPosition(750.f, 190.f);
    undoBtn.setFillColor(sf::Color(70, 70, 150));
    sf::Text undoTxt("UNDO", font, 24);
    undoTxt.setPosition(785.f, 200.f);

    sf::RectangleShape redoBtn({140.f, 50.f});
    redoBtn.setPosition(750.f, 260.f);
    redoBtn.setFillColor(sf::Color(70, 150, 70));
    sf::Text redoTxt("REDO", font, 24);
    redoTxt.setPosition(785.f, 270.f);
    // =============================================

    sf::Text msg("", font, 20);
    msg.setPosition(750.f, 340.f);

    sf::Text score("", font, 22);
    score.setPosition(750.f, 400.f);

    constexpr float boardMaxSize = 600.f;
    const float cellSize = boardMaxSize / static_cast<float>(N - 1);
    const float boardSize = cellSize * static_cast<float>(N - 1);

    while (window.isOpen()) {
        sf::Event e{};
        while (window.pollEvent(e)) {

            if (e.type == sf::Event::Closed)
                window.close();

            if (e.type == sf::Event::MouseButtonPressed &&
                e.mouseButton.button == sf::Mouse::Left) {

                sf::Vector2f m(
                    static_cast<float>(e.mouseButton.x),
                    static_cast<float>(e.mouseButton.y)
                );

                if (exitBtn.getGlobalBounds().contains(m)) {
                    window.close();
                }
                else if (undoBtn.getGlobalBounds().contains(m)) {
                    try { partida.undo(); } catch (...) {}
                }
                else if (redoBtn.getGlobalBounds().contains(m)) {
                    try { partida.redo(); } catch (...) {}
                }
                else if (passBtn.getGlobalBounds().contains(m)) {
                    try {
                        partida.aplicaMutare(Mutare(Pozitie{0,0}, tipM::pass));
                        if (!partida.esteIncheiat())
                            partida.aplicaMutare(
                                j2->alegeMutare(partida.getTabla())
                            );
                    } catch (...) {}
                }
                else {
                    constexpr float offsetX = 50.f;
                    const float offsetY =
                        (static_cast<float>(window.getSize().y) - boardSize) / 2.f;

                    const float rx = m.x - offsetX;
                    const float ry = m.y - offsetY;

                    if (rx >= -cellSize/2 && ry >= -cellSize/2 &&
                        rx <= boardSize + cellSize/2 &&
                        ry <= boardSize + cellSize/2) {

                        auto col =
                            static_cast<unsigned int>(std::round(rx / cellSize));
                        auto row =
                            static_cast<unsigned int>(std::round(ry / cellSize));

                        if (row < N && col < N) {
                            try {
                                partida.aplicaMutare(
                                    Mutare(Pozitie{row, col}, tipM::plasare)
                                );
                                if (!partida.esteIncheiat())
                                    partida.aplicaMutare(
                                        j2->alegeMutare(partida.getTabla())
                                    );
                            } catch (const GoException& ex) {
                                std::cout << ex.what() << "\n";
                            }
                        }
                    }
                }
            }
        }

        // determinare castigator
        if (partida.esteIncheiat()) {
            msg.setString(
                "Joc terminat!\n" +
                partida.determinaCastigator()
            );
        } else {
            msg.setString(
                partida.getTurnActual() == Culoare::Negru
                ? "Randul tau"
                : "AlphaGo gandeste..."
            );
        }

        // sugerare mutare
        if (!partida.esteIncheiat() &&
            partida.getTurnActual() == Culoare::Negru) {

            auto sugestie = partida.sugereazaMutare();
            if (sugestie.has_value()) {
                msg.setString(
                    msg.getString() +
                    "\nSugestie: (" +
                    std::to_string(sugestie->x) + ", " +
                    std::to_string(sugestie->y) + ")"
                );
            }
        }

        std::stringstream ss;
        ss << "SCOR\n"
           << j1->getNume() << ": " << partida.getCapturateNegru()
           << "\nBot: "
           << static_cast<float>(partida.getCapturateAlb()) + 6.5f;
        score.setString(ss.str());

        window.clear(sf::Color(100, 70, 70));
        partida.getTabla().draw(window);

        window.draw(exitBtn); window.draw(exitTxt);
        window.draw(passBtn); window.draw(passTxt);
        window.draw(undoBtn); window.draw(undoTxt);
        window.draw(redoBtn); window.draw(redoTxt);
        window.draw(msg);
        window.draw(score);

        window.display();
    }

    return 0;

#endif
}
