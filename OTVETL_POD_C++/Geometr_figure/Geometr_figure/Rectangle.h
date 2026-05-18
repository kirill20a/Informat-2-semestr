#ifndef RECTANGLE_H
#define RECTANGLE_H

#include "Shape.h"

class Rectangle : public Shape {
private:
    double width;
    double height;

public:
    Rectangle(double w, double h) : width(w), height(h) {}

    Rectangle(const Point& p1, const Point& p2, const Point& p3, const Point& p4) {
        width = p1.distanceTo(p2);
        height = p2.distanceTo(p3);
    }

    double calc_area() const override {
        return width * height;
    }

    double calc_perimetr() const override {
        return 2 * (width + height);
    }

    string get_name() const override {
        return "Прямоугольник";
    }
};

#endif // RECTANGLE_H