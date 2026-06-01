#ifndef BOARD_H
#define BOARD_H

#include "Position.h"
#include "Figure.h"
#include <vector>
#include <string>
#include <fstream>
#include <iostream>

struct Move {
    Position from;
    Position to;
    Figure* figure;
    Figure* captured;
};

class Board {
private:
    std::vector<std::vector<Figure*>> board;
    Position whiteKingPos;
    Position blackKingPos;

public:
    Board();
    ~Board();

    void setFigure(Position pos, Figure* figure);
    Figure* getFigureAt(Position pos) const;
    void removeFigure(Position pos);
    void clear();

    Position getKingPos(bool white) const {
        return white ? whiteKingPos : blackKingPos;
    }

    // Эти методы НЕ const, потому что они временно меняют доску при проверке
    bool isSquareAttacked(Position pos, bool byWhite) const;
    bool isCheck(bool whiteKing);
    bool isCheckmate(bool whiteKing);

    void addFigureFromString(const std::string& figStr, bool isWhite);
    void loadFromFile(const std::string& filename);

    // Новые методы
    bool isMateInOne(bool whiteMoves);
    std::vector<Move> getMovesThatLeadToMate(bool whiteMoves);

    void print() const;
};

#endif