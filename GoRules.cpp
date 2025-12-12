#include "GoRules.hpp"

const double GoRules::KOMI_VALUE = 6.5;
int GoRules::getBoardSize() {
    return DEFAULT_BOARD_SIZE;
}

double GoRules::getKomi() {
    return KOMI_VALUE;
}

int GoRules::getFortifiedStoneCost() {
    return FORTIFIED_STONE_COST;
}

bool GoRules::isKoViolation() {
    return false;
}

bool GoRules::canBeCaptured() {
    return true;
}




