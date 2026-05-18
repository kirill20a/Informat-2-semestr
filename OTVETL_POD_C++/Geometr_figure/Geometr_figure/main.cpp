#include <iostream>
#include <fstream>
#include <string>
#include <queue>
#include <vector>
#include <cmath>
#include <algorithm>
#include <clocale>

#include "Shape.h"
#include "Circle.h"
#include "Rectangle.h"
#include "Triangle.h"
#include "Polygon.h"

using namespace std;

const int ROWS = 200;
const int COLS = 200;

struct Region {
    int minX, maxX, minY, maxY;
    int pointCount;
    vector<pair<int, int>> points;

    Region() : minX(COLS), maxX(-1), minY(ROWS), maxY(-1), pointCount(0) {}

    void addPoint(int x, int y) {
        points.push_back(make_pair(x, y));
        pointCount++;
        minX = min(minX, x);
        maxX = max(maxX, x);
        minY = min(minY, y);
        maxY = max(maxY, y);
    }

    int getWidth() const { return maxX - minX + 1; }
    int getHeight() const { return maxY - minY + 1; }

    // ========== ПРОВЕРКА НА ПРЯМОУГОЛЬНИК ==========
    bool isRectangle() const {
        int expected = getWidth() * getHeight();
        return pointCount == expected;
    }

    // ========== ПРОВЕРКА НА ЭЛЛИПС/КРУГ (через симметрию) ==========
    bool isEllipse() const {
        if (pointCount < 30) return false;

        int cx = (minX + maxX) / 2;
        int cy = (minY + maxY) / 2;

        int symmetricPoints = 0;

        for (size_t i = 0; i < points.size(); i++) {
            int x = points[i].first;
            int y = points[i].second;

            // Симметричная точка относительно центра
            int rx = cx - (x - cx);
            int ry = cy - (y - cy);

            // Проверяем наличие симметричной точки
            for (size_t j = 0; j < points.size(); j++) {
                if (abs(points[j].first - rx) <= 1 && abs(points[j].second - ry) <= 1) {
                    symmetricPoints++;
                    break;
                }
            }
        }

        // Если больше 70% точек имеют симметричные пары — это эллипс/круг
        return (double)symmetricPoints / pointCount > 0.95;
    }

    // ========== ПРОВЕРКА НА ТРЕУГОЛЬНИК ==========
    bool isTriangle() const {
        if (pointCount < 5) return false;

        // Количество точек в каждой строке (по Y)
        vector<int> pointsPerRow(maxY - minY + 1, 0);
        for (size_t i = 0; i < points.size(); i++) {
            int y = points[i].second;
            pointsPerRow[y - minY]++;
        }

        // Для треугольника количество точек в строке сначала растёт, потом убывает
        bool increasing = true;
        int prev = 0;
        int nonZeroCount = 0;

        for (size_t i = 0; i < pointsPerRow.size(); i++) {
            int current = pointsPerRow[i];
            if (current == 0) continue;

            nonZeroCount++;

            if (increasing) {
                if (current < prev) {
                    increasing = false;
                }
            }
            else {
                if (current > prev) {
                    return false;
                }
            }
            prev = current;
        }

        return nonZeroCount >= 3;
    }

    // ========== ОПРЕДЕЛЕНИЕ ТИПА ФИГУРЫ ==========
    string getType() const {
        // Сначала отсеиваем слишком маленькие области (шум)
        if (pointCount < 10) return "Noise";

        // Проверяем прямоугольник
        if (isRectangle()) return "Rectangle";

        // Проверяем круг/эллипс
        if (isEllipse()) return "Circle";

        // Проверяем треугольник
        if (isTriangle()) return "Triangle";

        return "Noise";
    }
};

// ========== BFS ДЛЯ ПОИСКА СВЯЗНОЙ ОБЛАСТИ ==========
void bfs(const vector<string>& grid, vector<vector<bool>>& visited, int startX, int startY, Region& region) {
    queue<pair<int, int>> q;
    q.push(make_pair(startX, startY));
    visited[startY][startX] = true;

    int dx[] = { 1, -1, 0, 0, 1, 1, -1, -1 };
    int dy[] = { 0, 0, 1, -1, 1, -1, 1, -1 };

    while (!q.empty()) {
        pair<int, int> current = q.front();
        q.pop();

        int x = current.first;
        int y = current.second;

        region.addPoint(x, y);

        for (int i = 0; i < 8; i++) {
            int nx = x + dx[i];
            int ny = y + dy[i];

            if (nx >= 0 && nx < COLS && ny >= 0 && ny < ROWS &&
                !visited[ny][nx] && grid[ny][nx] == '1') {
                visited[ny][nx] = true;
                q.push(make_pair(nx, ny));
            }
        }
    }
}

// ========== ПОДСЧЁТ ФИГУР В ФАЙЛЕ ==========
void countFiguresInFile(const string& inputFile, int& rectangles, int& circles, int& triangles, int& noise, double& totalArea) {
    vector<string> grid(ROWS);
    ifstream input(inputFile.c_str());

    for (int i = 0; i < ROWS && getline(input, grid[i]); i++) {}
    input.close();

    vector<vector<bool>> visited(ROWS, vector<bool>(COLS, false));

    rectangles = 0;
    circles = 0;
    triangles = 0;
    noise = 0;
    totalArea = 0;

    for (int y = 0; y < ROWS; y++) {
        for (int x = 0; x < COLS; x++) {
            if (!visited[y][x] && grid[y][x] == '1') {
                Region region;
                bfs(grid, visited, x, y, region);
                if (region.pointCount < 20) {
                    noise++;
                    continue;
                }
                string type = region.getType();

                if (type == "Rectangle") {
                    rectangles++;
                    totalArea += region.getWidth() * region.getHeight();
                }
                else if (type == "Triangle") {
                    triangles++;
                    // Упрощённая площадь треугольника через bounding box
                    totalArea += (region.getWidth() * region.getHeight()) / 2.0;
                }
                else if (type == "Circle") {
                    circles++;
                    double radius = (region.getWidth() + region.getHeight()) / 4.0;
                    totalArea += M_PI * radius * radius;
                }
                else {
                    noise++;
                }
            }
        }
    }
}

// ========== СРАВНЕНИЕ С ЭТАЛОННЫМ ФАЙЛОМ ==========
bool compareWithOutput(const string& outputFile, int rectangles, int circles, int triangles, int noise) {
    ifstream file(outputFile.c_str());
    if (!file.is_open()) {
        cout << "  Не удалось открыть эталонный файл: " << outputFile << endl;
        return false;
    }

    int expectedRect = -1, expectedCircle = -1, expectedTriangle = -1, expectedNoise = 0;

    file >> expectedRect >> expectedCircle >> expectedTriangle >> expectedNoise;
    file.close();

    if (expectedRect == -1) {
        file.open(outputFile.c_str());
        string line;
        int lineNum = 0;
        while (getline(file, line)) {
            if (lineNum == 0) expectedRect = stoi(line);
            else if (lineNum == 1) expectedCircle = stoi(line);
            else if (lineNum == 2) expectedTriangle = stoi(line);
            else if (lineNum == 3) expectedNoise = stoi(line);
            lineNum++;
        }
        file.close();
    }

    bool success = true;

    if (expectedRect != -1 && expectedRect != rectangles) {
        cout << "  Rectangle: получено " << rectangles << ", ожидалось " << expectedRect << "-" << endl;
        success = false;
    }
    else if (expectedRect != -1) {
        cout << "  Rectangle: " << rectangles << "+ " << endl;
    }

    if (expectedCircle != -1 && expectedCircle != circles) {
        cout << "  Circle: получено " << circles << ", ожидалось " << expectedCircle << "-" << endl;
        success = false;
    }
    else if (expectedCircle != -1) {
        cout << "  Circle: " << circles << "+ " << endl;
    }

    if (expectedTriangle != -1 && expectedTriangle != triangles) {
        cout << "  Triangle: получено " << triangles << ", ожидалось " << expectedTriangle << "-" << endl;
        success = false;
    }
    else if (expectedTriangle != -1) {
        cout << "  Triangle: " << triangles << " +" << endl;
    }

    if (expectedNoise != -1 && expectedNoise != noise) {
        cout << "  Noise: получено " << noise << endl;
        success = false;
    }
    else if (expectedNoise != -1) {
        cout << "  Noise: " << noise << " +" << endl;
    }

    return success;
}
void calculateLandArea() {
    cout << "\n=== Площадь земельного участка ===" << endl;

    
    // Указатель на базовый класс Shape указывает на объект Polygon
    Shape* land = new Polygon("granitsy-uchastka2.txt");

    // Виртуальный вызов — определяется во время выполнения
    cout << "Фигура: " << land->get_name() << endl;
    cout << "Файл: granitsy-uchastka2.txt" << endl;
    cout << "Количество вершин: " << dynamic_cast<Polygon*>(land)->getVertexCount() << endl;
    cout << "Периметр участка: " << land->calc_perimetr() << " единиц" << endl;
    cout << "Площадь участка: " << land->calc_area() << " кв. единиц" << endl;

    delete land;  // освобождаем память
}

// ========== ДЕМОНСТРАЦИЯ С РАЗНЫМИ ФИГУРАМИ ==========
void demonstratePolymorphism() {
    cout << "\n=== ДЕМОНСТРАЦИЯ  ===" << endl;
    cout << "----------------------------------------" << endl;

    // Массив указателей на базовый класс Shape
    Shape* shapes[4];

    // Заполняем разными фигурами
    shapes[0] = new Circle(5);
    shapes[1] = new Rectangle(4, 6);
    shapes[2] = new Triangle(3, 4, 5);
    shapes[3] = new Polygon("granitsy-uchastka2.txt");

    // Полиморфный вызов — каждый объект ведёт себя по-своему
    for (int i = 0; i < 4; i++) {
        cout << shapes[i]->get_name()
            << ": площадь = " << shapes[i]->calc_area()
            << ", периметр = " << shapes[i]->calc_perimetr() << endl;
        delete shapes[i];  // освобождаем память
    }
}

// ========== MAIN ==========
int main() {
    setlocale(LC_ALL, "Russian");

    cout << "=== Геометрические фигуры ===" << endl;
    cout << "========================================" << endl;

    // Демонстрация
    demonstratePolymorphism();

    // Распознавание фигур из входных файлов
    string files[] = { "1", "2", "3" };
    bool allSuccess = true;
    double totalAreaAll = 0;

    for (int i = 0; i < 3; i++) {
        string inputFile = "input" + files[i] + ".dat";
        string outputFile = "output" + files[i] + ".dat";

        cout << "\n--- Проверка " << inputFile << " ---" << endl;

        int rectangles, circles, triangles, noise;
        double totalArea;
        countFiguresInFile(inputFile, rectangles, circles, triangles, noise, totalArea);

        cout << "Результат распознавания:" << endl;
        cout << "  Rectangle = " << rectangles << endl;
        cout << "  Circle = " << circles << endl;
        cout << "  Triangle = " << triangles << endl;
        cout << "  Noise = " << noise << endl;
        cout << "  Площадь фигур: " << totalArea << endl;

        totalAreaAll += totalArea;

        cout << "Сравнение с эталоном (" << outputFile << "):" << endl;
        bool success = compareWithOutput(outputFile, rectangles, circles, triangles, noise);
        if (!success) allSuccess = false;
    }

    cout << "\n=== Общая площадь всех фигур ===" << endl;
    cout << "Суммарная площадь: " << totalAreaAll << " кв. единиц" << endl;

    calculateLandArea();

    cout << "\n========================================" << endl;
    if (allSuccess) {
        cout << "+ Все тесты распознавания пройдены успешно!" << endl;
    }
    else {
        cout << "- Некоторые тесты распознавания не пройдены" << endl;
    }

    return 0;
}