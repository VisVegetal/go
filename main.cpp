#include "Joc.hpp"
#include "GoExceptions.hpp"
#include <iostream>

int main() {
    try {
        Jucator* j1 = new JucatorUman("Andrei", Culoare::Negru);
        Jucator* j2 = new JucatorBot("BotV1", Culoare::Alb);

        Joc partida(Dimensiuni::D9x9, j1, j2);
        std::cout << "Partide: " << Joc::getStatisticaJocuri() << "\n";
        partida.joaca();
    } catch (const std::exception& e) {
        std::cerr << "Eroare: " << e.what() << "\n";
    }
    return 0;
}