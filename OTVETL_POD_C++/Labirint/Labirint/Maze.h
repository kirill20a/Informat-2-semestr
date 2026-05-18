#ifndef MAZE_H
#define MAZE_H

#include <iostream>
#include <vector>
#include <string>

// “очка на плоскости (координаты пиксел€)
struct Point {
    int x, y;
    Point(int x = 0, int y = 0) : x(x), y(y) {}

    bool operator==(const Point& other) const;
    bool operator!=(const Point& other) const;
};

// ¬ывод точки в поток (дл€ отладки)
std::ostream& operator<<(std::ostream& out, const Point& p);

// ќсновной класс лабиринта
class Maze {
private:
    std::vector<uint8_t> pixels;   // массив пикселей (RGB)
    int width, height;             // размеры изображени€
    int rowSize;                   // выровненна€ ширина строки BMP
    Point start, end;              // вход и выход (зелЄный / красный кружки)
    std::vector<Point> path;       // найденный путь

    bool readBMP(const std::string& filename);          // загрузить BMP
    bool writeBMP(const std::string& filename) const;   // сохранить BMP
    void findStartEnd();                                 // найти старт и финиш по цвету
    void buildGrid(std::vector<std::vector<int>>& grid, int brightnessThreshold) const; // построить карту проходимости
    bool waveAlgo(int brightnessThreshold);              // волновой алгоритм (Lee)
    void drawPath(int thickness = 2);                    // нарисовать путь (синим)

public:
    Maze();
    bool load(const std::string& filename);             // загрузить лабиринт
    bool solve(int brightnessThreshold = 90);           // решить лабиринт
    bool save(const std::string& filename) const;       // сохранить результат
    void printInfo() const;                             // вывести информацию

    // перегрузка операторов
    bool operator!() const;          // true, если путь не найден
    explicit operator bool() const;   // true, если путь найден
};

#endif