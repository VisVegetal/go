#include "GUI.hpp"
#include <iostream>

GUI::GUI() : window(sf::VideoMode(800, 800), "Go Game - C++ POO Project") {
    window.setFramerateLimit(60);
}

void GUI::displayBoard(const Board& board) const {
    window.clear(sf::Color(220, 179, 92));
    drawGrid(board.getSize());

    for (int i = 0; i < board.getSize(); ++i) {
        for (int j = 0; j < board.getSize(); ++j) {
            BoardObject* obj = board.getObject(i, j);
            if (obj) {
                bool isFort = (dynamic_cast<FortifiedStone*>(obj) != nullptr);
                drawStone(i, j, obj->getColor(), isFort);
            }
        }
    }
    window.display();
}

void GUI::drawGrid(int size) const {
    for (int i = 0; i < size; ++i) {
        sf::RectangleShape hLine(sf::Vector2f(cellSize * (size - 1), 2));
        hLine.setPosition(margin, margin + i * cellSize);
        hLine.setFillColor(sf::Color::Black);
        window.draw(hLine);

        sf::RectangleShape vLine(sf::Vector2f(2, cellSize * (size - 1)));
        vLine.setPosition(margin + i * cellSize, margin);
        vLine.setFillColor(sf::Color::Black);
        window.draw(vLine);
    }
}

void GUI::drawStone(int row, int col, ObjectColor color, bool fortified) const {
    sf::CircleShape stone(cellSize * 0.45f);
    stone.setOrigin(stone.getRadius(), stone.getRadius());
    stone.setPosition(margin + col * cellSize, margin + row * cellSize);

    if (color == ObjectColor::Black)
        stone.setFillColor(sf::Color::Black);
    else
        stone.setFillColor(sf::Color::White);

    if (fortified) {
        stone.setOutlineThickness(3);
        stone.setOutlineColor(sf::Color::Red);
    }
    window.draw(stone);
}

std::pair<int, int> GUI::getMoveInput(const std::string& playerName) const {
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
                return {-2, -2};
            }
            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                float x = event.mouseButton.x;
                float y = event.mouseButton.y;

                int col = std::round((x - margin) / cellSize);
                int row = std::round((y - margin) / cellSize);

                return {row, col};
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::P)) return {-1, -1}; // Pass
        }
    }
    return {-2, -2};
}

void GUI::displayWelcomeMessage() const { std::cout << "Welcome to SFML Go!\n"; }
void GUI::displayMessage(const std::string& msg) const { std::cout << msg << "\n"; }
void GUI::displayError(const CustomGoException& ex) const { std::cerr << "Error: " << ex.what() << "\n"; }
void GUI::displayScore(int b, int w) const { std::cout << "Black: " << b << " White: " << w << "\n"; }
bool GUI::askForFortifiedStone(const std::string& p, int c) const {
    std::cout << p << ", press F for Fortified, N for Normal: ";
    // Simulare rapidă:
    return sf::Keyboard::isKeyPressed(sf::Keyboard::F);
}