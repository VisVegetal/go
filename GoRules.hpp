#ifndef GO_RULES_HPP
#define GO_RULES_HPP

class GoRules {
private:
    GoRules() = delete;
    static const int DEFAULT_BOARD_SIZE = 19;
    static const double KOMI_VALUE;
    static const int FORTIFIED_STONE_COST = 2;
public:
    static int getBoardSize();
    static double getKomi();
    static int getFortifiedStoneCost();
    static bool isKoViolation();
    static bool canBeCaptured();
};


#endif