#ifndef FUNCTION_H
#define FUNCTION_H

#include <iostream>
#include <cmath>
#include <vector>

using namespace std;

// Абстрактный класс "Функция"
class Function {
public:
    virtual ~Function() {}

    // вычисление значения y = f(x)
    virtual double value(double x) const = 0;

    // вывод формулы функции
    virtual void print() const = 0;

    // поиск минимума на интервале [a, b] (простой перебор)
    double findMin(double a, double b, double step = 0.001) const {
        double minVal = value(a);
        for (double x = a; x <= b; x += step) {
            double y = value(x);
            if (y < minVal) minVal = y;
        }
        return minVal;
    }

    // поиск максимума на интервале [a, b]
    double findMax(double a, double b, double step = 0.001) const {
        double maxVal = value(a);
        for (double x = a; x <= b; x += step) {
            double y = value(x);
            if (y > maxVal) maxVal = y;
        }
        return maxVal;
    }

    // численное интегрирование (метод трапеций)
    double integrate(double a, double b, int steps = 1000) const {
        double h = (b - a) / steps;
        double sum = 0.5 * (value(a) + value(b));
        for (int i = 1; i < steps; i++) {
            sum += value(a + i * h);
        }
        return sum * h;
    }

    // численное дифференцирование
    double derivative(double x, double h = 1e-6) const {
        return (value(x + h) - value(x - h)) / (2 * h);
    }
};

#endif