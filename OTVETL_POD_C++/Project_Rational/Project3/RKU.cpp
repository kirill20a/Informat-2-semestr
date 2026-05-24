#include <iostream>
#include <cmath>
#include "Rational.h"

using namespace std;

int main() {
    setlocale(LC_ALL, "Russian");

    Rational a, b, c;

    cout << "Введите коэффициенты квадратного уравнения ax² + bx + c = 0" << endl;
    cout << "Формат ввода: числитель/знаменатель (например, 1 1 , -3532 13923 , 209 13923)" << endl;

    cout << "a = "; cin >> a;
    cout << "b = "; cin >> b;
    cout << "c = "; cin >> c;

    cout << "Уравнение: " << a << "x² + " << b << "x + " << c << " = 0" << endl;

    double da = double(a);
    double db = double(b);
    double dc = double(c);

    double D = db * db - 4 * da * dc;

    cout << "Дискриминант (double): " << D << endl;

    if (D < 0) {
        cout << "Нет действительных корней" << endl;
    }
    else if (D == 0) {
        double x = -db / (2 * da);
        cout << "Один корень: " << x << endl;
    }
    else {
        double x1 = (-db - sqrt(D)) / (2 * da);
        double x2 = (-db + sqrt(D)) / (2 * da);
        cout << "Два корня: " << x1 << " и " << x2 << endl;
    }

    return 0;
}