#ifndef FIELD_H
#define FIELD_H

#include <iostream>

using namespace std;

// –азмеры пол€
const int WIDTH = 7;
const int HEIGHT = 6;

// —осто€ние €чейки
enum Cell {
    EMPTY,
    RED,
    YELLOW
};

class Field {
private:
    Cell cells[WIDTH][HEIGHT];
    bool isRedTurn;   // чей сейчас ход (true Ц красный)
    Cell winner;      // победитель (EMPTY Ц нет)

    void checkWinner(); // проверка победы после хода

public:
    Field(bool redFirst = true);
    void clear(bool redFirst = true);
    bool makeTurn(int column);
    bool isOver() const;
    Cell getCell(int x, int y) const;
    bool isRedTurnNow() const;
    Cell getWinner() const;
    void print() const;
    void printResult() const;
};

#endif