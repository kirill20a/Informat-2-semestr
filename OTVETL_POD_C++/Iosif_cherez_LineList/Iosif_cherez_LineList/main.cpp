#include <iostream>
#include <fstream>
#include <ctime>
#include "LineList.h"

using namespace std;

// Функция задачи Иосифа с использованием LineList
int Josephus(int n, int k) {
    LineList circle;

    // Заполняем список числами от 1 до n
    for (int i = 1; i <= n; i++) {
        circle.push_back(i);
    }

    int currentIndex = 0;

    while (circle.getSize() > 1) {
        // Находим индекс для удаления (с шагом k)
        currentIndex = (currentIndex + k - 1) % circle.getSize();
        // Удаляем элемент
        circle.removeAt(currentIndex);
        // currentIndex теперь указывает на следующий элемент
    }

    return circle[0];
}

int main() {
    setlocale(LC_ALL, "Russian");

    // Открываем файл для записи результатов
    ofstream file("josephus_results.txt");
    file << "N\tAnswer\tTime(sec)" << endl;

    // Массив значений N для тестирования
    int values[] = {
        1000, 5000, 10000, 15000, 30000, 50000,
        100000, 200000, 300000, 400000, 500000,
        600000, 700000, 800000, 900000, 1000000
    };
    int k = 2;
    int numTests = sizeof(values) / sizeof(values[0]);

    cout << "=== Задача Иосифа с LineList ===" << endl;
    cout << "N\t\tОтвет\t\tВремя (сек)" << endl;
    cout << "--------------------------------------------" << endl;

    for (int i = 0; i < numTests; i++) {
        int n = values[i];

        clock_t start = clock();
        int result = Josephus(n, k);
        clock_t end = clock();

        double duration = (double)(end - start) / CLOCKS_PER_SEC;

        cout << n << "\t\t" << result << "\t\t" << duration << endl;
        file << n << "\t" << result << "\t" << duration << endl;
    }

    file.close();
    cout << "\n✅ Результаты сохранены в файл 'josephus_results.txt'" << endl;
    cout << "\nПрограмма завершена. Нажмите Enter для выхода...";
    cin.get();
    return 0;
}