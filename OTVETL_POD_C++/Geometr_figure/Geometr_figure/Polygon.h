#ifndef POLYGON_H
#define POLYGON_H

#include "Shape.h"
#include <fstream>

class Polygon : public Shape {
private:
    Point vertices[100];
    int vertexCount;

public:
    Polygon() : vertexCount(0) {}

    Polygon(const Point* points, int count) : vertexCount(count) {
        for (int i = 0; i < count && i < 100; i++) {
            vertices[i] = points[i];
        }
    }

    //  онструктор из файла
    Polygon(const string& filename) : vertexCount(0) {
        ifstream file(filename.c_str());
        if (!file.is_open()) return;

        int n;
        file >> n;  // перва€ строка Ч количество точек

        double x, y;
        for (int i = 0; i < n && i < 100; i++) {
            file >> x >> y;
            vertices[vertexCount++] = Point(x, y);
        }
        file.close();
    }

    double calc_area() const override {
        double area = 0;
        for (int i = 0; i < vertexCount; i++) {
            int j = (i + 1) % vertexCount;
            area += vertices[i].getX() * vertices[j].getY();
            area -= vertices[j].getX() * vertices[i].getY();
        }
        return abs(area) / 2;
    }

    double calc_perimetr() const override {
        double per = 0;
        for (int i = 0; i < vertexCount; i++) {
            int j = (i + 1) % vertexCount;
            per += vertices[i].distanceTo(vertices[j]);
        }
        return per;
    }

    string get_name() const override {
        return "ћногоугольник";
    }

    void addPoint(const Point& p) {
        if (vertexCount < 100) {
            vertices[vertexCount++] = p;
        }
    }

    // ========== ¬ќ«¬–јў®ЌЌјя ‘”Ќ ÷»я ==========
    int getVertexCount() const {
        return vertexCount;
    }
};

#endif