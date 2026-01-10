#include <SFML/Graphics.hpp>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <memory> // Adaugat pentru std::unique_ptr (rezolva erorile de memorie)
#include "Joc.hpp"
#include "GoExceptions.hpp"

int main() {
    //initializare jucator uman
    std::string numeJucator;
    std::cout << "Introdu numele tau: ";
    if (!(std::getline(std::cin, numeJucator)) || numeJucator.empty()) {
        numeJucator = "Jucator1";
    }

    //initializare fereastra sfml
    //rezolutie + titlu fereastra
    sf::RenderWindow window(sf::VideoMode(1000, 750), "Go Game - OOP Project");
    window.setFramerateLimit(60);

    //fonturi externe
    sf::Font font;
    if (!font.loadFromFile("C:/Windows/Fonts/arial.ttf")) {
        std::cerr << "Eroare criticala: Fontul nu a putut fi incarcat!\n";
        return -1;
    }

    auto j1_ptr = std::make_unique<JucatorUman>(numeJucator, Culoare::Negru);
    auto j2_ptr = std::make_unique<JucatorBot>("AlphaGo", Culoare::Alb);

    Jucator* j1 = j1_ptr.get();
    Jucator* j2 = j2_ptr.get();

    Joc partida(Dimensiuni::D9x9, j1, j2);

    //User Interface
    //configurare buton PASS
    sf::RectangleShape passBtn(sf::Vector2f(140.0f, 50.0f));
    passBtn.setPosition(650.0f, 100.0f);
    passBtn.setFillColor(sf::Color::Red);
    passBtn.setOutlineThickness(2.0f);

    sf::Text textPass("PASS", font, 24);
    textPass.setPosition(695.0f, 110.0f);

    //creare buton EXIT
    sf::RectangleShape exitBtn(sf::Vector2f(140.0f, 50.0f));
    exitBtn.setPosition(820.0f, 660.0f);
    exitBtn.setFillColor(sf::Color(50, 50, 50));
    exitBtn.setOutlineThickness(2.0f);

    sf::Text textExit("EXIT", font, 24);
    textExit.setPosition(865.0f, 670.0f);

    //texte pentru scor si mesaje
    sf::Text uiText("", font, 22);
    uiText.setPosition(650.0f, 200.0f);

    sf::Text msgText("", font, 20);
    msgText.setPosition(650.0f, 450.0f);

    //game loop
    while (window.isOpen()) {
        sf::Event event{};
        //gestionare input tastatura/mouse
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            //detectare click pe ecran
            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                sf::Vector2f mPos(static_cast<float>(event.mouseButton.x), static_cast<float>(event.mouseButton.y));
                //verificare exit
                if (exitBtn.getGlobalBounds().contains(mPos)) {
                    window.close();
                }
                //executarea jocului daca partida nu s-a incheiat
                if (!partida.esteIncheiat()) {
                    try {
                        //daca se apasa pass
                        if (passBtn.getGlobalBounds().contains(mPos)) {
                            partida.aplicaMutare(Mutare({0, 0}, tipM::pass));
                            if (!partida.esteIncheiat()) {
                                partida.aplicaMutare(j2->alegeMutare(partida.getTabla()));
                            }
                        }
                        //utilizatorul plaseaza o piatra
                        else {
                            // Rezolvare: Mutat in inner scope pentru Clang-Tidy
                            constexpr float cellSize = 40.0f;
                            constexpr float offset = 50.0f;

                            //convertire coordonate pixeli in coordonate grila
                            auto col = static_cast<unsigned int>((mPos.x - offset + 20.0f) / cellSize);
                            auto row = static_cast<unsigned int>((mPos.y - offset + 20.0f) / cellSize);

                            //executarea mutarii
                            if (row < 9 && col < 9) {
                                // Daca mutarea e ilegala, catch-ul va prinde exceptia si botul nu va muta.
                                partida.aplicaMutare(Mutare({row, col}, tipM::plasare));

                                if (!partida.esteIncheiat()) {
                                    partida.aplicaMutare(j2->alegeMutare(partida.getTabla()));
                                }
                            }
                        }
                    } catch (const GoException& e) {
                        //tratarea erorilor (mesajul apare in consola, dar jocul continua)
                        std::cout << "Eroare Joc: " << e.what() << std::endl;
                    }
                }
            }
        }

        Jucator* jucatorCurent = (partida.getTurnActual() == Culoare::Negru) ? j1 : j2;
        auto* om = dynamic_cast<JucatorUman*>(jucatorCurent);

        if (om != nullptr) {
            msgText.setString("Este randul tau, " + om->getNume() + "!");
            msgText.setFillColor(sf::Color::Cyan);
        } else {
            msgText.setString("AlphaGo se gandeste...");
            msgText.setFillColor(sf::Color::Yellow);
        }

        //actualizare scor
        std::stringstream ss;
        ss << std::fixed << std::setprecision(1);
        ss << "SCOR:\n" << j1->getNume() << ": " << partida.getCapturateNegru()
           << "\nBot: " << static_cast<float>(partida.getCapturateAlb()) + 6.5f;

        if (partida.esteIncheiat()) {
            ss << "\n\n" << partida.determinaCastigator();
        }
        uiText.setString(ss.str());

        //randare
        window.clear(sf::Color(104, 75, 75));
        partida.getTabla().draw(window);

        //desenare elemente UI
        window.draw(passBtn);
        window.draw(exitBtn);
        window.draw(textPass);
        window.draw(textExit);
        window.draw(uiText);
        window.draw(msgText);
        window.display(); //afisare
    }

    //curatare si export
    if (partida.esteIncheiat()) {
        if (auto* bot = dynamic_cast<JucatorBot*>(j2)) {
            bot->exportaStatistici();
        }
    }
    return 0;
}