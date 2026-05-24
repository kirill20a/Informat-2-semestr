#include "Rational.h"
#include <iostream>
#include <clocale>
using namespace std;

int main2() {
    setlocale(LC_ALL, "ru");

    //Конструкторы
    Rational a(1, 2), b(-1, 6);
    cout << "a = " << a << ", b = " << b << endl;
    cout << "a + b = " << a + b << endl;
    cout << "a - b = " << a - b << endl;
    cout << "a * b = " << a * b << endl;
    cout << "a / b = " << a / b << endl;

    cout << "--------------" << endl;

    //Составные операторы
    Rational c = a;
    c += b;
    cout << "a += b = " << c << endl;

    Rational d = a;
    d -= b;
    cout << "a -= b = " << d << endl;

    cout << "--------------" << endl;

    //Унарные операторы
    cout << "-b = " << -b << endl;

    Rational e(2, 3);
    cout << "e = " << e << endl;
    cout << "++e = " << ++e << endl;
    cout << "e++ = " << e++ << ", затем e = " << e << endl;
    cout << "--e = " << --e << endl;
    cout << "e-- = " << e-- << ", затем e = " << e << endl;

    cout << "--------------" << endl;

    //Операторы сравнения
    Rational f(1, 2), g(2, 4);
    cout << "f = " << f << ", g = " << g << endl;
    cout << "f == g: " << (f == g) << endl;
    cout << "f != g: " << (f != g) << endl;
    cout << "f < g: " << (f < g) << endl;
    cout << "f > g: " << (f > g) << endl;
    cout << "f <= g: " << (f <= g) << endl;
    cout << "f >= g: " << (f >= g) << endl;

    cout << "--------------" << endl;

    //Преобразования типов
    Rational h(5, 2);
    cout << "h = " << h << endl;
    cout << "int(h) = " << int(h) << endl;
    cout << "double(h) = " << double(h) << endl;

    cout << "--------------" << endl;

    //Конструктора из double
    Rational fromDouble(3.75);
    cout << "3.75 = " << fromDouble << " = " << double(fromDouble) << endl;

    Rational fromDouble2(2.333333);
    cout << "2.333333 = " << fromDouble2 << endl;

    cout << "--------------" << endl;

    //Квадратное уравнение
    Rational root1, root2;
    int rootCount = root1.solveQuadratic(1, -5, 6, root1, root2);

    cout << "Уравнение: x^2 - 5x + 6 = 0" << endl;
    if (rootCount == 2)
        cout << "Корни: " << root1 << " и " << root2 << endl;
    else if (rootCount == 1)
        cout << "Корень: " << root1 << endl;
    else
        cout << "Нет рациональных корней" << endl;

    cout << "--------------" << endl;

    // Уравнение с 1 корнем
    rootCount = root1.solveQuadratic(1, -4, 4, root1, root2);
    cout << "Уравнение: x^2 - 4x + 4 = 0" << endl;
    if (rootCount == 2)
        cout << "Корни: " << root1 << " и " << root2 << endl;
    else if (rootCount == 1)
        cout << "Корень: " << root1 << endl;
    else
        cout << "Нет рациональных корней" << endl;

    cout << "--------------" << endl;

    // Ввод клава
    Rational input, compare(1, 3);
    cout << "Введите рациональное число (числитель и знаменатель через пробел): ";
    cin >> input;
    cout << "Вы ввели: " << input << endl;
    cout << "Сравнение с 1/3: " << (input == compare ? "равно" : "не равно") << endl;

    return 0;
}