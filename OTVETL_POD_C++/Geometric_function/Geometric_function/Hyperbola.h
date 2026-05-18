#ifndef HYPERBOLA_H
#define HYPERBOLA_H

#include "Function.h"

class Hyperbola : public Function {
private:
    double k;
public:
    Hyperbola(double k) : k(k) {}

    double value(double x) const override {
        if (x == 0) return 1e9; // избегаем деления на ноль
        return k / x;
    }

    void print() const override {
        cout << "y = " << k << " / x";
    }
};

#endif