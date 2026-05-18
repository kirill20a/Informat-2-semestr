#ifndef CIRCLE_H
#define CIRCLE_H

#include "Shape.h"

class Circle : public Shape {
private:
    Point center;
    double radius;

public:
    Circle(const Point& c, double r) : center(c), radius(r) {}
    Circle(double cx, double cy, double r) : center(cx, cy), radius(r) {}
    Circle(double r) : center(0, 0), radius(r) {}

    double calc_area() const override {
        return M_PI * radius * radius;
    }

    double calc_perimetr() const override {
        return 2 * M_PI * radius;
    }

    string get_name() const override {
        return "Круг";
    }
};

#endif // CIRCLE_H