#ifndef ELLIPSE_H
#define ELLIPSE_H

#include "Shape.h"

class Ellipse : public Shape {
private:
    Point center;
    double radiusA;
    double radiusB;

public:
    Ellipse(const Point& c, double a, double b) : center(c), radiusA(a), radiusB(b) {}
    Ellipse(double cx, double cy, double a, double b) : center(cx, cy), radiusA(a), radiusB(b) {}

    double calc_area() const override {
        return M_PI * radiusA * radiusB;
    }

    double calc_perimetr() const override {
        double h = pow((radiusA - radiusB), 2) / pow((radiusA + radiusB), 2);
        return M_PI * (radiusA + radiusB) * (1 + (3 * h) / (10 + sqrt(4 - 3 * h)));
    }

    string get_name() const override {
        return "έλλθορ";
    }
};

#endif // ELLIPSE_H