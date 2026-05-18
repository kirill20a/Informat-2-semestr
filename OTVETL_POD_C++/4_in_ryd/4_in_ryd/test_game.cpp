#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include "Field.h"

using namespace std;

string winnerName(const Field& field) {
    Cell w = field.getWinner();
    if (w == RED) return "Красный";
    if (w == YELLOW) return "Жёлтый";
    return "Ничья";
}

void testPredefinedMoves() {
    cout << "\n=== Тест 1: Заранее заданные ходы ===" << endl;
    Field field(true);
    int moves[] = { 1, 1, 1, 1 };
    for (int col : moves) field.makeTurn(col);
    field.print();
    cout << "Результат: " << winnerName(field) << endl;
}

void testRandomGames(int gameCount = 5) {
    cout << "\n=== Тест 2: Случайные игры ===" << endl;
    srand((unsigned)time(nullptr));
    for (int g = 1; g <= gameCount; g++) {
        Field field(true);
        int moves = 0;
        while (!field.isOver()) {
            int col = rand() % WIDTH + 1;
            field.makeTurn(col);
            moves++;
        }
        cout << "Игра " << g << ": " << winnerName(field)
            << ", ходов: " << moves << endl;
    }
    cout << "✅ Все случайные игры завершились без ошибок." << endl;
}

void testDraw() {
    cout << "\n=== Тест 3: Проверка ничьей ===" << endl;
    Field field(true);
    for (int i = 0; i < WIDTH * HEIGHT; i++) {
        int col = i % WIDTH + 1;
        field.makeTurn(col);
    }
    field.print();
    cout << "Результат: " << winnerName(field) << endl;
}

void testNoMoveAfterWin() {
    cout << "\n=== Тест 4: Запрет хода после победы ===" << endl;
    Field field(true);
    field.makeTurn(1);
    field.makeTurn(2);
    field.makeTurn(1);
    field.makeTurn(2);
    field.makeTurn(1);
    field.makeTurn(2);
    field.makeTurn(1);
    bool canMove = field.makeTurn(2);
    if (!canMove && field.getWinner() == RED && field.isOver()) {
        cout << "✅ Ход после победы заблокирован" << endl;
    }
    else {
        cout << "❌ Тест не пройден" << endl;
    }
}

int main2() {
    setlocale(LC_ALL, "Russian");
    cout << "===== ЗАПУСК ТЕСТОВ =====" << endl;
    testPredefinedMoves();
    testRandomGames(5);
    testDraw();
    testNoMoveAfterWin();
    cout << "\n===== ВСЕ ТЕСТЫ ЗАВЕРШЕНЫ =====" << endl;
    return 0;
}