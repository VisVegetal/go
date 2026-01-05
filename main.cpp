#include <iostream>
#include <limits>
#include "Joc.hpp"
#include "Jucator.hpp"
#include "GoExceptions.hpp"
#include "Culoare.hpp"

void curataInput() {
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

int main() {
    std::cout << "==========================================\n";
    std::cout << "           SA INCEAPA JOCUL GO            \n";
    std::cout << "==========================================\n";

    std::cout << std::endl;

    try {
        std::cout << "Alegeti tipul de partida:\n";
        std::cout << "1. Om vs Om\n";
        std::cout << "2. Om vs Bot\n";
        int optiune;
        if (!(std::cin >> optiune))
            throw GoException("Input invalid pentru tipul de joc");

        Jucator* p1 = new JucatorUman("Jucarot_Negru", Culoare::Negru);
        Jucator* p2 = nullptr;

        if (optiune == 1)
            p2 = new JucatorUman("Jucator_Alb", Culoare::Alb);
        else
            p2 = new JucatorBot("Jucator_Bot", Culoare::Alb);

        Joc partida(Dimensiuni::D9x9, p1, p2, 6.5f);

        if (dynamic_cast<JucatorBot*>(p2))
            std::cout << "INFO: Ati ales sa jucati impotriva unui algoritm\n";

        std::cout << "STATISTICA: Partide creaate in aceasta sesiune: "
                  << Joc::getStatisticaJocuri() <<"\n\n";

        partida.joaca();
    }

    catch (const GoException& e) {
        std::cerr << "\n EROARE CRITICA JOC: " << e.what() << "\n";


    }
    catch (const std::exception& e) {
        std::cerr << "\n EROARE SISTEM: " << e.what() << "\n";

    }

    catch (...) {
        std::cerr << "EROARE NECUNOSCUTA: Programul se va inchide" << std::endl;
    }

    std::cout << "\n==========================================\n";
    std::cout << "   Program finalizat. Apasati Enter...    \n";
    std::cout << "==========================================\n";

    return 0;
}