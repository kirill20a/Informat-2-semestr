#ifndef MAZE_H
#define MAZE_H

#include <iostream>
#include <vector>
#include <string>

struct Point {
    int x, y;
    Point(int x = 0, int y = 0) : x(x), y(y) {}

    bool operator==(const Point& other) const;
    bool operator!=(const Point& other) const;
};

std::ostream& operator<<(std::ostream& out, const Point& p);

class Maze {
private:
    std::vector<uint8_t> pixels;
    int width, height;
    int rowSize;
    Point start, end;
    std::vector<Point> path;

    bool readBMP(const std::string& filename);
    bool writeBMP(const std::string& filename) const;
    void findStartEnd();
    void buildGrid(std::vector<std::vector<int>>& grid, int brightnessThreshold) const;
    bool waveAlgo(int brightnessThreshold);
    void drawPath(int thickness = 2);

public:
    Maze();
    bool load(const std::string& filename);
    bool solve(int brightnessThreshold = 90);
    bool save(const std::string& filename) const;
    void printInfo() const;

    // перегрузка операторов
    bool operator!() const;
    explicit operator bool() const;
};

#endif