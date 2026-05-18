#include <iostream>
#include <ctime>
#include "Array.h"

using namespace std;

// Функция задачи Иосифа
int Josephus(int n, int k) {
    Array circle(n);
    for (int i = 1; i <= n; i++) {
        circle.add(i);
    }

    int currentIndex = 0;
    while (circle.getSize() > 1) {
        currentIndex = (currentIndex + k - 1) % circle.getSize();
        circle.removeAt(currentIndex);
    }

    return circle[0];
}

int main() {
    setlocale(LC_ALL, "Russian");

    int values[] = { 5000000,8000000  };
    int k = 2;

    cout << "=== Задача Иосифа ===" << endl;
    cout << "N\t\tОтвет\t\tВремя (сек)" << endl;
    cout << "--------------------------------------------" << endl;

    for (int n : values) {
        clock_t start = clock();
        int result = Josephus(n, k);
        clock_t end = clock();

        double duration = (double)(end - start) / CLOCKS_PER_SEC;
        cout << n << "\t\t" << result << "\t\t" << duration << endl;
    }

    cout << "\nПрограмма завершена. Нажмите Enter для выхода...";
    cin.get();
    return 0;
}