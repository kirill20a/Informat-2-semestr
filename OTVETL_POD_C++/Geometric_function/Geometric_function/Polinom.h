#ifndef POLINOM_H
#define POLINOM_H

#include "Function.h"

class Polinom : public Function {
private:
    vector<double> coeffs; // coeffs[i] — коэффициент при x^i
public:
    // конструктор: передаём коэффициенты от младшей степени к старшей
    Polinom(const vector<double>& coeffs) : coeffs(coeffs) {}

    double value(double x) const override {
        double result = 0;
        double power = 1;
        for (size_t i = 0; i < coeffs.size(); i++) {
            result += coeffs[i] * power;
            power *= x;
        }
        return result;
    }

    void print() const override {
        cout << "y = ";
        for (int i = coeffs.size() - 1; i >= 0; i--) {
            if (coeffs[i] == 0) continue;
            if (i != (int)coeffs.size() - 1 && coeffs[i] > 0) cout << " + ";
            if (i == 0) cout << coeffs[i];
            else if (i == 1) cout << coeffs[i] << "x";
            else cout << coeffs[i] << "x^" << i;
        }
    }
};

#endif