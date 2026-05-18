#include "Field.h"

int main() {
    setlocale(LC_ALL, "Russian");

    Field field(true); // первым ходит красный
    int column;

    cout << "=== Игра 'Четыре в ряд' ===" << endl;
    cout << "Правила: введите номер колонки (1-7)" << endl;

    while (!field.isOver()) {
        field.print();
        cout << "Ходит " << (field.isRedTurnNow() ? "КРАСНЫЙ" : "ЖЁЛТЫЙ")
            << " игрок. Ваш ход: ";
        cin >> column;
        field.makeTurn(column);
    }

    field.print();
    field.printResult();

    return 0;
}