#ifndef PARABOLA_H
#define PARABOLA_H

#include "Function.h"

class Parabola : public Function {
private:
    double a, b, c;
public:
    Parabola(double a, double b, double c) : a(a), b(b), c(c) {}

    double value(double x) const override {
        return a * x * x + b * x + c;
    }

    void print() const override {
        cout << "y = " << a << "x² + " << b << "x + " << c;
    }
};

#endif