#include "Hyperbola.h"
#include "Parabola.h"
#include "Exponenta.h"
#include "Polinom.h"

int main() {
    setlocale(LC_ALL, "Russian");

    cout << "=== Демонстрация работы классов функций ===" << endl;

    // 1. Гипербола
    Hyperbola hyp(5.0);
    cout << "\n--- Гипербола ---" << endl;
    hyp.print();
    cout << "\nf(2) = " << hyp.value(2) << endl;
    cout << "Минимум на [1, 5]: " << hyp.findMin(1, 5) << endl;
    cout << "Максимум на [1, 5]: " << hyp.findMax(1, 5) << endl;
    cout << "Интеграл от 1 до 5: " << hyp.integrate(1, 5) << endl;
    cout << "Производная в x=2: " << hyp.derivative(2) << endl;

    // 2. Парабола
    Parabola par(2, -3, 1);
    cout << "\n--- Парабола ---" << endl;
    par.print();
    cout << "\nf(0) = " << par.value(0) << endl;
    cout << "Минимум на [-2, 3]: " << par.findMin(-2, 3) << endl;
    cout << "Максимум на [-2, 3]: " << par.findMax(-2, 3) << endl;
    cout << "Интеграл от -1 до 2: " << par.integrate(-1, 2) << endl;
    cout << "Производная в x=1: " << par.derivative(1) << endl;

    // 3. Экспонента
    Exponenta expFunc(2, 0.5);
    cout << "\n--- Экспонента ---" << endl;
    expFunc.print();
    cout << "\nf(0) = " << expFunc.value(0) << endl;
    cout << "Минимум на [-1, 1]: " << expFunc.findMin(-1, 1) << endl;
    cout << "Максимум на [-1, 1]: " << expFunc.findMax(-1, 1) << endl;
    cout << "Интеграл от 0 до 2: " << expFunc.integrate(0, 2) << endl;
    cout << "Производная в x=0: " << expFunc.derivative(0) << endl;

    // 4. Многочлен (полином)
    Polinom poly({ 1, -2, 0, 1 });  // 1 - 2x + 0x² + 1x³ = x³ - 2x + 1
    cout << "\n--- Многочлен ---" << endl;
    poly.print();
    cout << "\nf(2) = " << poly.value(2) << endl;
    cout << "Минимум на [-2, 2]: " << poly.findMin(-2, 2) << endl;
    cout << "Максимум на [-2, 2]: " << poly.findMax(-2, 2) << endl;
    cout << "Интеграл от -1 до 1: " << poly.integrate(-1, 1) << endl;
    cout << "Производная в x=1: " << poly.derivative(1) << endl;

    return 0;
}