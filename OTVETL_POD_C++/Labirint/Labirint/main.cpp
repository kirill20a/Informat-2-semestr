#include<clocale>
#include "Maze.h"

int main() {
    setlocale(LC_ALL, "Russian");
    Maze maze;

    if (!maze.load("ДНК_Лабиринт.bmp")) {
        std::cerr << "Ошибка загрузки BMP" << std::endl;
        return 1;
    }

    if (!maze.solve(90)) {
        std::cerr << "Путь не найден!" << std::endl;
        return 1;
    }

    maze.printInfo();
    maze.save("labirint_solution.bmp");

    if (maze) {
        std::cout << "Результат сохранён в labirint_solution.bmp" << std::endl;
    }

    return 0;
}