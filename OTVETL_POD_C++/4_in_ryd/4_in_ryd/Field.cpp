#include "Field.h"

// Конструктор
Field::Field(bool redFirst) {
    clear(redFirst);
}

// Очистка поля
void Field::clear(bool redFirst) {
    isRedTurn = redFirst;
    winner = EMPTY;
    for (int i = 0; i < WIDTH; i++)
        for (int j = 0; j < HEIGHT; j++)
            cells[i][j] = EMPTY;
}

// Проверка победителя
void Field::checkWinner() {
    const int di[] = { 1, 0, 1, 1 };  // направления: ↓, →, ↗, ↘
    const int dj[] = { 0, 1, 1, -1 };
    const int WIN = 4;

    for (int i = 0; i < WIDTH; i++) {
        for (int j = 0; j < HEIGHT; j++) {
            Cell start = cells[i][j];
            if (start == EMPTY) continue;

            for (int dir = 0; dir < 4; dir++) {
                int length = 1;
                int ni = i + di[dir];
                int nj = j + dj[dir];

                while (ni >= 0 && ni < WIDTH && nj >= 0 && nj < HEIGHT &&
                    cells[ni][nj] == start) {
                    length++;
                    if (length >= WIN) {
                        winner = start;
                        return;
                    }
                    ni += di[dir];
                    nj += dj[dir];
                }
            }
        }
    }
}

// Выполнение хода
bool Field::makeTurn(int column) {
    if (winner != EMPTY) return false;
    if (column < 1 || column > WIDTH) return false;

    int col = column - 1;
    for (int row = 0; row < HEIGHT; row++) {
        if (cells[col][row] == EMPTY) {
            cells[col][row] = isRedTurn ? RED : YELLOW;
            checkWinner();
            isRedTurn = !isRedTurn;
            return true;
        }
    }
    return false; // колонка заполнена
}

// Проверка, окончена ли игра
bool Field::isOver() const {
    if (winner != EMPTY) return true;
    for (int i = 0; i < WIDTH; i++)
        for (int j = 0; j < HEIGHT; j++)
            if (cells[i][j] == EMPTY)
                return false;
    return true; // ничья
}

// Получение ячейки
Cell Field::getCell(int x, int y) const {
    if (x >= 0 && x < WIDTH && y >= 0 && y < HEIGHT)
        return cells[x][y];
    return EMPTY;
}

// Чей ход
bool Field::isRedTurnNow() const {
    return isRedTurn;
}

// Победитель
Cell Field::getWinner() const {
    return winner;
}

// Печать поля
void Field::print() const {
    cout << "\n  1 2 3 4 5 6 7\n";
    for (int row = HEIGHT - 1; row >= 0; row--) {
        cout << row + 1 << " ";
        for (int col = 0; col < WIDTH; col++) {
            char c;
            if (cells[col][row] == RED) c = 'R';
            else if (cells[col][row] == YELLOW) c = 'Y';
            else c = '.';
            cout << c << " ";
        }
        cout << endl;
    }
    cout << endl;
}

// Печать результата
void Field::printResult() const {
    if (winner == RED)
        cout << "Победил красный игрок!" << endl;
    else if (winner == YELLOW)
        cout << "Победил жёлтый игрок!" << endl;
    else
        cout << "Ничья!" << endl;
}