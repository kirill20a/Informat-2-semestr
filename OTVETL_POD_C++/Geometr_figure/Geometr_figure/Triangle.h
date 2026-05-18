#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "Shape.h"

class Triangle : public Shape {
private:
    double a, b, c;

public:
    Triangle(double sideA, double sideB, double sideC) : a(sideA), b(sideB), c(sideC) {}

    Triangle(const Point& p1, const Point& p2, const Point& p3) {
        a = p1.distanceTo(p2);
        b = p2.distanceTo(p3);
        c = p3.distanceTo(p1);
    }

    double calc_area() const override {
        double p = (a + b + c) / 2;
        return sqrt(p * (p - a) * (p - b) * (p - c));
    }

    double calc_perimetr() const override {
        return a + b + c;
    }

    string get_name() const override {
        return "Треугольник";
    }

    bool is_valid() const {
        return (a + b > c) && (a + c > b) && (b + c > a);
    }
};

#endif // TRIANGLE_H