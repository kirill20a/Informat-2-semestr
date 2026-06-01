#ifndef FIGURE_H
#define FIGURE_H

#include "Position.h"
#include <vector>
#include <string>

class Board;  // forward declaration

// Абстрактный класс "Шахматная фигура"
class Figure {
protected:
    Position pos;      // текущая позиция
    bool isWhite;      // цвет: true = белые, false = чёрные
    char symbol;       // символ для вывода: 'K','Q','R','B','N','P'

public:
    Figure(Position pos, bool isWhite, char symbol)
        : pos(pos), isWhite(isWhite), symbol(symbol) {
    }

    virtual ~Figure() {}

    // Геттеры
    Position getPos() const { return pos; }
    bool getIsWhite() const { return isWhite; }
    char getSymbol() const { return symbol; }

    void setPos(Position newPos) { pos = newPos; }

    // Чисто виртуальные методы (обязательны для переопределения)
    virtual std::vector<Position> getRawMoves(const Board& board) const = 0;
    virtual std::string getName() const = 0;
    virtual char getChar() const { return symbol; }
};

#endif