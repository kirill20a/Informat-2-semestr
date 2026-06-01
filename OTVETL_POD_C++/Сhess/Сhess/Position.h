#ifndef POSITION_H
#define POSITION_H

#include <string>
#include <cstdlib>

// Структура для хранения координат шахматной клетки
// x = 0..7 (a..h), y = 0..7 (1..8)
struct Position {
    int x, y;

    Position(int x = 0, int y = 0) : x(x), y(y) {}

    // Проверка, что координаты в пределах доски
    bool isValid() const {
        return x >= 0 && x < 8 && y >= 0 && y < 8;
    }

    bool operator==(const Position& other) const {
        return x == other.x && y == other.y;
    }

    bool operator!=(const Position& other) const {
        return !(*this == other);
    }

    // Преобразование из шахматной нотации "A4" в координаты
    static Position fromString(const std::string& str) {
        if (str.length() < 2) return Position(-1, -1);
        int x = str[0] - 'A';
        int y = str[1] - '1';
        return Position(x, y);
    }

    // Преобразование координат в шахматную нотацию
    std::string toString() const {
        std::string s;
        s += char('A' + x);
        s += char('1' + y);
        return s;
    }
};

#endif