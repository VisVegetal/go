#include <algorithm>
#include <cmath>
#include <iostream>
#include <memory>
#include <optional>
#include <sstream>
#include <string>

#include "../include/GoExceptions.hpp"
#include "../include/Joc.hpp"

#ifndef GO_HEADLESS
#include <SFML/Graphics.hpp>
#endif

#if defined(CH_CI_RUN) || defined(GO_HEADLESS)
// Test de fum headless: doua boturi joaca o partida scurta, exercitand
// regulile (capturi, sinucidere, Ko) fara a fi nevoie de interfata grafica.
static int ruleazaSmokeTestHeadless() {
    auto botNegru = std::make_unique<JucatorBot>(
        "Bot Negru", Culoare::Negru, creeazaStrategie(NivelBot::Usor));
    auto botAlb = std::make_unique<JucatorBot>(
        "Bot Alb", Culoare::Alb, creeazaStrategie(NivelBot::Mediu));

    Joc partida(Dimensiuni::D9x9, *botNegru, *botAlb);

    for (int i = 0; i < 60 && !partida.esteIncheiat(); ++i) {
        auto& curent = (partida.getTurnActual() == Culoare::Negru)
                           ? botNegru
                           : botAlb;

        const Mutare mutare = curent->alegeMutare(partida);
        if (!partida.esteMutareValida(mutare)) {
            std::cout << "ERROR: botul a returnat o mutare invalida!\n";
            return 1;
        }
        partida.aplicaMutare(mutare);
    }

    std::cout << partida.determinaCastigator() << "\n";
    return 0;
}
#endif

#ifndef GO_HEADLESS
// Configuratia aleasa in meniul de start.
struct Configuratie {
    Dimensiuni dim;
    NivelBot nivel;
    Culoare culoareJucator; // Negru sau Alb
};

namespace {

// Antialiasing (8x) pentru margini netede ale pietrelor si butoanelor.
sf::ContextSettings setariAntialiasing() {
    sf::ContextSettings s;
    s.antialiasingLevel = 8;
    return s;
}

// Versiune mai luminoasa a culorii, pentru starea de hover.
sf::Color culoareHover(const sf::Color& baza) {
    return sf::Color(
        std::min(255, baza.r + 35),
        std::min(255, baza.g + 35),
        std::min(255, baza.b + 35),
        baza.a);
}

// Centreaza un text peste dreptunghiul butonului.
void centreazaText(sf::Text& text, const sf::FloatRect& dreptunghi) {
    const auto b = text.getLocalBounds();
    text.setOrigin(b.left + b.width / 2.f, b.top + b.height / 2.f);
    text.setPosition(dreptunghi.left + dreptunghi.width / 2.f,
                     dreptunghi.top + dreptunghi.height / 2.f);
}

// Buton reutilizabil, cu text centrat si stare vizuala de hover.
struct Buton {
    sf::RectangleShape shape;
    sf::Text text;
    sf::Color normal;
    sf::Color hover;

    bool contine(const sf::Vector2f& m) const {
        return shape.getGlobalBounds().contains(m);
    }

    void aplicaHover(bool peste) {
        shape.setFillColor(peste ? hover : normal);
        shape.setOutlineThickness(peste ? 3.f : 2.f);
        shape.setOutlineColor(peste ? sf::Color(230, 230, 230)
                                    : sf::Color(100, 100, 100));
    }

    void marcheazaSelectat() {
        shape.setFillColor(sf::Color(0, 130, 60));
        shape.setOutlineThickness(3.f);
        shape.setOutlineColor(sf::Color::White);
    }
};

Buton creeazaButon(const sf::Font& font, float x, float y, float w, float h,
                   const std::string& eticheta, unsigned int marimeFont,
                   sf::Color culoare) {
    Buton b;
    b.shape.setSize({w, h});
    b.shape.setPosition(x, y);
    b.normal = culoare;
    b.hover = culoareHover(culoare);
    b.shape.setFillColor(b.normal);
    b.shape.setOutlineThickness(2.f);
    b.shape.setOutlineColor(sf::Color(100, 100, 100));

    b.text.setFont(font);
    b.text.setString(eticheta);
    b.text.setCharacterSize(marimeFont);
    b.text.setFillColor(sf::Color::White);
    centreazaText(b.text, b.shape.getGlobalBounds());
    return b;
}

} // namespace anonim

// Meniu GUI: dimensiunea tablei, nivelul botului si culoarea jucatorului.
static Configuratie alegeConfiguratieGUI(sf::Font& font) {
    sf::RenderWindow menu(sf::VideoMode(620, 520), "Configurare joc - Go",
                          sf::Style::Default, setariAntialiasing());
    menu.setFramerateLimit(60);

    const auto titlu = [&](float y, const std::string& text) {
        sf::Text t(text, font, 24);
        t.setPosition(50.f, y);
        t.setFillColor(sf::Color(230, 230, 230));
        return t;
    };

    const sf::Text titleDim = titlu(20.f, "Alege dimensiunea tablei:");
    const sf::Text titleNivel = titlu(150.f, "Alege nivelul botului:");
    const sf::Text titleCuloare = titlu(280.f, "Alege culoarea:");

    auto b9 = creeazaButon(font, 50.f, 70.f, 160.f, 55.f, "9 x 9", 20,
                           sf::Color(70, 70, 70));
    auto b13 = creeazaButon(font, 230.f, 70.f, 160.f, 55.f, "13 x 13", 20,
                            sf::Color(70, 70, 70));
    auto b19 = creeazaButon(font, 410.f, 70.f, 160.f, 55.f, "19 x 19", 20,
                            sf::Color(70, 70, 70));

    auto bUsor = creeazaButon(font, 50.f, 200.f, 160.f, 55.f, "Usor", 20,
                              sf::Color(70, 70, 70));
    auto bMediu = creeazaButon(font, 230.f, 200.f, 160.f, 55.f, "Mediu", 20,
                               sf::Color(70, 70, 70));
    auto bAvansat = creeazaButon(font, 410.f, 200.f, 160.f, 55.f, "Avansat", 20,
                                 sf::Color(70, 70, 70));

    auto bNegru = creeazaButon(font, 50.f, 330.f, 160.f, 55.f, "Negru", 20,
                               sf::Color(35, 35, 35));
    auto bAlb = creeazaButon(font, 230.f, 330.f, 160.f, 55.f, "Alb", 20,
                             sf::Color(210, 210, 210));
    bAlb.text.setFillColor(sf::Color(40, 40, 40)); // text inchis pe fond deschis

    Configuratie config{Dimensiuni::D9x9, NivelBot::Mediu, Culoare::Negru};
    std::optional<Dimensiuni> dimAleasa;
    std::optional<NivelBot> nivelAles;
    std::optional<Culoare> culoareAlesa;

    while (menu.isOpen()) {
        sf::Event e{};
        while (menu.pollEvent(e)) {
            if (e.type == sf::Event::Closed) {
                menu.close();
                return config;
            }

            if (e.type == sf::Event::MouseButtonPressed &&
                e.mouseButton.button == sf::Mouse::Left) {
                const sf::Vector2f m(static_cast<float>(e.mouseButton.x),
                                     static_cast<float>(e.mouseButton.y));

                if (!dimAleasa.has_value()) {
                    if (b9.contine(m)) dimAleasa = Dimensiuni::D9x9;
                    else if (b13.contine(m)) dimAleasa = Dimensiuni::D13x13;
                    else if (b19.contine(m)) dimAleasa = Dimensiuni::D19x19;
                }
                if (dimAleasa.has_value() && !nivelAles.has_value()) {
                    if (bUsor.contine(m)) nivelAles = NivelBot::Usor;
                    else if (bMediu.contine(m)) nivelAles = NivelBot::Mediu;
                    else if (bAvansat.contine(m)) nivelAles = NivelBot::Avansat;
                }
                if (dimAleasa.has_value() && nivelAles.has_value() &&
                    !culoareAlesa.has_value()) {
                    if (bNegru.contine(m)) culoareAlesa = Culoare::Negru;
                    else if (bAlb.contine(m)) culoareAlesa = Culoare::Alb;
                }
            }
        }

        if (dimAleasa.has_value() && nivelAles.has_value() &&
            culoareAlesa.has_value()) {
            config.dim = *dimAleasa;
            config.nivel = *nivelAles;
            config.culoareJucator = *culoareAlesa;
            menu.close();
            continue;
        }

        // Culori afisate: selectat > hover > normal.
        const auto m = static_cast<sf::Vector2f>(sf::Mouse::getPosition(menu));
        const auto actualizeaza = [&](Buton& btn, bool selectat) {
            if (selectat) {
                btn.marcheazaSelectat();
            } else {
                btn.aplicaHover(btn.contine(m));
            }
        };

        actualizeaza(b9, dimAleasa == Dimensiuni::D9x9);
        actualizeaza(b13, dimAleasa == Dimensiuni::D13x13);
        actualizeaza(b19, dimAleasa == Dimensiuni::D19x19);
        actualizeaza(bUsor, nivelAles == NivelBot::Usor);
        actualizeaza(bMediu, nivelAles == NivelBot::Mediu);
        actualizeaza(bAvansat, nivelAles == NivelBot::Avansat);
        actualizeaza(bNegru, culoareAlesa == Culoare::Negru);
        actualizeaza(bAlb, culoareAlesa == Culoare::Alb);

        menu.clear(sf::Color(30, 30, 32));
        menu.draw(titleDim);
        menu.draw(titleNivel);
        menu.draw(titleCuloare);

        menu.draw(b9.shape); menu.draw(b9.text);
        menu.draw(b13.shape); menu.draw(b13.text);
        menu.draw(b19.shape); menu.draw(b19.text);

        menu.draw(bUsor.shape); menu.draw(bUsor.text);
        menu.draw(bMediu.shape); menu.draw(bMediu.text);
        menu.draw(bAvansat.shape); menu.draw(bAvansat.text);

        menu.draw(bNegru.shape); menu.draw(bNegru.text);
        menu.draw(bAlb.shape); menu.draw(bAlb.text);

        menu.display();
    }

    return config;
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

    const Configuratie config = alegeConfiguratieGUI(font);
    const auto N = static_cast<unsigned int>(config.dim);

    // Botul preia culoarea opusa jucatorului. In Go, Negrul muta mereu primul.
    const Culoare culoareBot =
        (config.culoareJucator == Culoare::Negru) ? Culoare::Alb : Culoare::Negru;

    sf::RenderWindow window(sf::VideoMode(1280, 800), "Go - Proiect POO",
                            sf::Style::Default, setariAntialiasing());
    window.setFramerateLimit(60);

    auto j1 = std::make_unique<JucatorUman>("Jucator", config.culoareJucator);
    auto j2 = std::make_unique<JucatorBot>(
        "Bot", culoareBot, creeazaStrategie(config.nivel));

    Joc partida(config.dim, *j1, *j2);

    // ------------------------- panou dreapta -------------------------
    constexpr float panelX = 730.f;
    constexpr float panelW = 550.f;
    constexpr float bx = panelX + 35.f;
    constexpr float btnW = panelW - 70.f;
    constexpr float btnH = 48.f;

    sf::RectangleShape panou({panelW, 800.f});
    panou.setPosition(panelX, 0.f);
    panou.setFillColor(sf::Color(48, 48, 52));

    auto passBtn = creeazaButon(font, bx, 90.f, btnW, btnH, "PASS", 22,
                                sf::Color(0, 120, 65));
    auto undoBtn = creeazaButon(font, bx, 150.f, btnW, btnH, "UNDO", 22,
                                sf::Color(70, 100, 180));
    auto redoBtn = creeazaButon(font, bx, 210.f, btnW, btnH, "REDO", 22,
                                sf::Color(60, 140, 140));
    auto cedeazaBtn = creeazaButon(font, bx, 270.f, btnW, btnH, "CEDARE", 22,
                                   sf::Color(175, 115, 30));
    auto exitBtn = creeazaButon(font, bx, 330.f, btnW, btnH, "EXIT", 22,
                                sf::Color(160, 45, 45));

    sf::RectangleShape scoreBox({btnW, 130.f});
    scoreBox.setPosition(bx, 400.f);
    scoreBox.setFillColor(sf::Color(35, 35, 38));
    scoreBox.setOutlineThickness(2.f);
    scoreBox.setOutlineColor(sf::Color(140, 140, 140));

    sf::Text msg("", font, 22);
    msg.setPosition(bx, 20.f);
    msg.setFillColor(sf::Color(235, 235, 235));

    sf::Text eroare("", font, 18);
    eroare.setPosition(bx, 560.f);
    eroare.setFillColor(sf::Color(255, 190, 90));

    sf::Text score("", font, 20);
    score.setPosition(bx + 15.f, 415.f);
    score.setFillColor(sf::Color(225, 225, 225));
    // -----------------------------------------------------------

    // D.R.Y.: o singura functie de apelare a turei botului, folosita
    // dupa mutarea omului, dupa PASS, dupa UNDO/REDO sau dupa CEDARE.
    const auto rundaBot = [&]() {
        if (partida.esteIncheiat()) return;
        if (partida.getTurnActual() == j1->getCuloare()) return; // randul omului
        const Mutare mutareBot = j2->alegeMutare(partida);
        partida.aplicaMutare(mutareBot);
    };

    constexpr float boardMaxSize = 600.f;
    const float cellSize = boardMaxSize / static_cast<float>(N - 1);
    const float boardSize = cellSize * static_cast<float>(N - 1);
    constexpr float offsetX = 50.f;
    const float offsetY =
        (static_cast<float>(window.getSize().y) - boardSize) / 2.f;

    // Transforma pozitia mouse-ului intr-o intersectie a tablei.
    const auto pozitieMouse = [&](const sf::Vector2f& m) -> std::optional<Pozitie> {
        const float rx = m.x - offsetX;
        const float ry = m.y - offsetY;
        if (rx < -cellSize / 2 || ry < -cellSize / 2 ||
            rx > boardSize + cellSize / 2 || ry > boardSize + cellSize / 2) {
            return std::nullopt;
        }
        const auto col = static_cast<unsigned int>(std::round(rx / cellSize));
        const auto row = static_cast<unsigned int>(std::round(ry / cellSize));
        if (row >= N || col >= N) return std::nullopt;
        return Pozitie{row, col};
    };

    // Daca omul a ales Alb, botul (Negru) muta primul, imediat ce se
    // deschide fereastra cu tabla. Negrul muta mereu primul in Go.
    if (config.culoareJucator == Culoare::Alb) {
        rundaBot();
    }

    while (window.isOpen()) {
        sf::Event e{};
        while (window.pollEvent(e)) {
            if (e.type == sf::Event::Closed) {
                window.close();
            }

            if (e.type == sf::Event::MouseButtonPressed &&
                e.mouseButton.button == sf::Mouse::Left) {

                const sf::Vector2f m(static_cast<float>(e.mouseButton.x),
                                     static_cast<float>(e.mouseButton.y));

                eroare.setString("");

                if (exitBtn.contine(m)) {
                    window.close();
                } else if (undoBtn.contine(m)) {
                    partida.undo();
                    rundaBot();
                } else if (redoBtn.contine(m)) {
                    partida.redo();
                    rundaBot();
                } else if (cedeazaBtn.contine(m)) {
                    partida.terminaJoc(config.culoareJucator == Culoare::Negru);
                } else if (passBtn.contine(m)) {
                    try {
                        partida.aplicaMutare(Mutare({0, 0}, tipM::pass));
                        rundaBot();
                    } catch (const GoException& ex) {
                        eroare.setString(ex.what());
                    }
                } else if (const auto pz = pozitieMouse(m); pz.has_value()) {
                    try {
                        partida.aplicaMutare(Mutare(*pz, tipM::plasare));
                        rundaBot();
                    } catch (const GoException& ex) {
                        eroare.setString(ex.what());
                    }
                }
            }
        }

        // Hover states pentru butoanele din panou.
        const auto m = static_cast<sf::Vector2f>(sf::Mouse::getPosition(window));
        passBtn.aplicaHover(passBtn.contine(m));
        undoBtn.aplicaHover(undoBtn.contine(m));
        redoBtn.aplicaHover(redoBtn.contine(m));
        cedeazaBtn.aplicaHover(cedeazaBtn.contine(m));
        exitBtn.aplicaHover(exitBtn.contine(m));

        // Piatra fantoma: previzualizeaza mutarea la hover pe tabla.
        std::optional<GhostStone> fantoma;
        if (!partida.esteIncheiat() &&
            partida.getTurnActual() == j1->getCuloare()) {
            const auto pz = pozitieMouse(m);
            if (pz.has_value() &&
                partida.esteMutareValida(Mutare(*pz, tipM::plasare))) {
                fantoma = GhostStone{
                    sf::Vector2f(offsetX + static_cast<float>(pz->y) * cellSize,
                                 offsetY + static_cast<float>(pz->x) * cellSize),
                    j1->getCuloare()};
            }
        }

        // ------------------------- stare mesaje -------------------------
        if (partida.esteIncheiat()) {
            const auto rezultat = partida.determinaCastigator();
            const auto poz = rezultat.find("\nCastigator:");
            msg.setString("Joc terminat!\n" +
                          ((poz != std::string::npos)
                               ? rezultat.substr(poz + 1)
                               : rezultat));
        } else {
            msg.setString(
                partida.getTurnActual() == j1->getCuloare()
                    ? "Randul tau"
                    : (std::string("Bot (") + j2->getNumeStrategie() +
                       ") gandeste..."));
        }

        if (!partida.esteIncheiat() &&
            partida.getTurnActual() == j1->getCuloare()) {
            Pozitie sugestie{0, 0};
            if (partida.sugereazaMutare(sugestie)) {
                msg.setString(
                    msg.getString() +
                    "\nSugestie: (" +
                    std::to_string(sugestie.x) + ", " +
                    std::to_string(sugestie.y) + ")");
            }
        }

        std::stringstream ss;
        const bool j1Negru = (config.culoareJucator == Culoare::Negru);
        const int captJ1 = j1Negru ? partida.getCapturateNegru()
                                   : partida.getCapturateAlb();
        const int captJ2 = j1Negru ? partida.getCapturateAlb()
                                   : partida.getCapturateNegru();
        const float scorJ1 =
            static_cast<float>(captJ1) + (j1Negru ? 0.f : partida.getKomi());
        const float scorJ2 =
            static_cast<float>(captJ2) + (j1Negru ? partida.getKomi() : 0.f);

        ss << "SCOR\n"
           << j1->getNume() << " (" << (j1Negru ? "Negru" : "Alb") << "): "
           << scorJ1 << "\n"
           << j2->getNume() << " (" << j2->getNumeStrategie() << "): "
           << scorJ2;
        score.setString(ss.str());

        // ------------------------- randare -------------------------
        window.clear(sf::Color(40, 40, 42)); // gri inchis elegant
        partida.getTabla().draw(window, fantoma);

        window.draw(panou);
        window.draw(passBtn.shape); window.draw(passBtn.text);
        window.draw(undoBtn.shape); window.draw(undoBtn.text);
        window.draw(redoBtn.shape); window.draw(redoBtn.text);
        window.draw(cedeazaBtn.shape); window.draw(cedeazaBtn.text);
        window.draw(exitBtn.shape); window.draw(exitBtn.text);
        window.draw(scoreBox);
        window.draw(msg);
        window.draw(eroare);
        window.draw(score);

        window.display();
    }

    return 0;

#endif
}
