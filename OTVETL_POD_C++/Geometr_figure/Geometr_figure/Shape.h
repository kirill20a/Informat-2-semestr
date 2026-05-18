#ifndef SHAPE_H
#define SHAPE_H

#include <iostream>
#include <string>
#include <cmath>
#include <set>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
using namespace std;

// ==========  À¿—— “Œ◊ ¿ ==========
class Point {
private:
    double x, y;

public:
    Point(double x = 0, double y = 0) : x(x), y(y) {}

    double getX() const { return x; }
    double getY() const { return y; }

    double distanceTo(const Point& other) const {
        double dx = x - other.x;
        double dy = y - other.y;
        return sqrt(dx * dx + dy * dy);
    }

    bool operator==(const Point& other) const {
        return x == other.x && y == other.y;
    }

    bool operator<(const Point& other) const {
        if (x != other.x) return x < other.x;
        return y < other.y;
    }
};

// ========== ¿¡—“–¿ “Õ€…  À¿—— SHAPE ==========
class Shape {
public:
    virtual ~Shape() {}

    virtual double calc_area() const = 0;
    virtual double calc_perimetr() const = 0;
    virtual string get_name() const = 0;
};

#endif // SHAPE_H