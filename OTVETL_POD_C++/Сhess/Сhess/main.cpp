#include "Board.h"
#include <iostream>

using namespace std;

int main() {
    setlocale(LC_ALL, "Russian");

    Board board;
    board.loadFromFile("position.txt");

    cout << "=== Текущая позиция ===" << endl;
    board.print();

    // Проверяем мат в 1 ход
    if (board.isMateInOne(true)) {
        cout << "\n✅ Найден мат в 1 ход!" << endl;
    }
    else {
        cout << "\n❌ Мата в 1 ход нет" << endl;

        // Проверяем мат в 2 хода
        vector<Move> moves = board.getMovesThatLeadToMate(true);
        if (!moves.empty()) {
            cout << "\n✅ Найден мат в 2 хода!" << endl;
            cout << "Ход, ведущий к мату: "
                << moves[0].from.toString() << " -> "
                << moves[0].to.toString() << endl;
        }
        else {
            cout << "\n❌ Мата в 2 хода нет" << endl;
        }
    }

    return 0;
}