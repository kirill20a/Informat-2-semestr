#include "Graph.h"
#include <clocale>
#include <iostream>

using namespace std;

void processFile(const string& filename, const string& prefix) {
    cout << "\n=== Обработка файла: " << filename << " ===" << endl;

    // II: конструктор из файла
    Graph graph(filename);

    // III: проверка BFS (если есть вершины 0 и 38)
    Node* start = nullptr;
    Node* end = nullptr;
    for (Node* node : graph) {
        if (node->getName() == "0") start = node;
        if (node->getName() == "38") end = node;
    }

    if (start && end) {
        BFS bfs(graph);
        bool pathExists = bfs.connected(start, end);
        cout << "Путь между 0 и 38: " << (pathExists ? "существует" : "не существует") << endl;
        if (pathExists) {
            cout << "Расстояние: " << bfs.findDistance(start, end) << " рёбер" << endl;
        }
    }
    else {
        cout << "Вершины 0 или 38 не найдены в графе" << endl;
    }

    // IV и V: выделение компонент связности
    auto components = graph.findConnectedComponents();

    cout << "Найдено компонент связности: " << components.size() << endl;

    // Сохраняем компоненты
    for (size_t i = 0; i < components.size(); ++i) {
        string filenameOut = prefix + "_component_" + to_string(i + 1) + ".txt";
        graph.saveComponent(components[i], filenameOut);
        cout << "  Компонента " << i + 1 << ": " << components[i].size() << " вершин -> " << filenameOut << endl;
    }
}

int main() {
    setlocale(LC_ALL, "Russian");

    // Задания I–IV: TestGraph.txt
    processFile("TestGraph.txt", "test");

    // Задание V: 1000.csv
    processFile("1000.csv", "data");

    return 0;
}