#include "Game.hpp"
#include "HumanPlayer.hpp"
#include "AIPlayer.hpp"
#include "GUI.hpp"
#include "GoExceptions.hpp"
#include <iostream>

#include "GUI.hpp"

int main() {
    try {
        Interface* gui = new GUI();

        Player* p1 = new HumanPlayer("Regina", StoneColor::Black);
        Player* p2 = new AIPlayer("Botu", StoneColor::White, 2);

        Game goGame(p1, p2, gui);
        goGame.startGame();
    } catch (const GameSetupException& e) {
        std::cerr << "Eroare critica la configurare: " << e.what() << std::endl;
        return 1;
    } catch (const std::exception& e) {
        std::cerr << "Eroare neprevazuta: " << e.what() << std::endl;
        return 2;
    }

    return 0;

}
