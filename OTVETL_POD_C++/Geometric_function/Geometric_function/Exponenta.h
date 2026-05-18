#ifndef EXPONENTA_H
#define EXPONENTA_H

#include "Function.h"

class Exponenta : public Function {
private:
    double a, b;
public:
    Exponenta(double a, double b) : a(a), b(b) {}

    double value(double x) const override {
        return a * exp(b * x);
    }

    void print() const override {
        cout << "y = " << a << " * exp(" << b << " * x)";
    }
};

#endif