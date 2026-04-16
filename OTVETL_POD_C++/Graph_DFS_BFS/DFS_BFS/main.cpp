#include <iostream>
#include "Graph.h"

using namespace std;

int main() {
    setlocale(LC_ALL, "Russian");

    cout << "=== ДЕМОНСТРАЦИЯ РАБОТЫ С ГРАФАМИ ===" << endl;
    cout << "=====================================" << endl << endl;

    // ========== СОЗДАНИЕ ГРАФА ==========
    Graph graph;

    // Создаём вершины
    Node* n0 = new Node("0");
    Node* n1 = new Node("1");
    Node* n2 = new Node("2");
    Node* n3 = new Node("3");
    Node* n4 = new Node("4");
    Node* n5 = new Node("5");

    // Добавляем вершины в граф
    graph.addNode(n0);
    graph.addNode(n1);
    graph.addNode(n2);
    graph.addNode(n3);
    graph.addNode(n4);
    graph.addNode(n5);

    // Добавляем рёбра
    graph.addEdge(n0, n1, 10);
    graph.addEdge(n1, n2, 15);
    graph.addEdge(n2, n5, 30);
    graph.addEdge(n0, n5, 50);
    graph.addEdge(n0, n3, 20);
    graph.addEdge(n3, n4, 25);
    graph.addEdge(n4, n5, 15);
    graph.addEdge(n1, n3, 10);
    graph.addEdge(n2, n4, 5);

    // ========== ДЕМОНСТРАЦИЯ ОПЕРАТОРА << ДЛЯ ГРАФА ==========
    cout << "=== ВЫВОД ГРАФА (operator<<) ===" << endl;
    cout << graph << endl;

    // ========== ДЕМОНСТРАЦИЯ ОПЕРАТОРОВ СРАВНЕНИЯ ДЛЯ NODE ==========
    cout << "=== СРАВНЕНИЕ ВЕРШИН (operator==, operator!=, operator<) ===" << endl;
    cout << "n0 == n0: " << (*n0 == *n0 ? "Да" : "Нет") << endl;
    cout << "n0 != n1: " << (*n0 != *n1 ? "Да" : "Нет") << endl;
    cout << "n0 < n1: " << (*n0 < *n1 ? "Да" : "Нет") << endl;
    cout << endl;

    // ========== ДЕМОНСТРАЦИЯ ОПЕРАТОРА << ДЛЯ NODE ==========
    cout << "=== ВЫВОД ВЕРШИНЫ (operator<<) ===" << endl;
    cout << "Вершина: " << *n0 << endl;
    cout << endl;

    // ========== ДЕМОНСТРАЦИЯ ОПЕРАТОРА ПРИСВАИВАНИЯ ДЛЯ NODE ==========
    cout << "=== ПРИСВАИВАНИЕ ВЕРШИНЫ (operator=) ===" << endl;
    Node n_copy("temp");
    n_copy = *n0;
    cout << "Копия вершины: " << n_copy << endl;
    cout << endl;

    // ========== BFS ==========
    cout << "=== ПОИСК В ШИРИНУ (BFS) ===" << endl;
    cout << "----------------------------------------" << endl;

    BFS bfs(graph);

    cout << "Существует ли путь между 0 и 5? "
        << (bfs.connected(n0, n5) ? "Да" : "Нет") << endl;

    cout << "Существует ли путь между 0 и 2? "
        << (bfs.connected(n0, n2) ? "Да" : "Нет") << endl;

    // ========== ДЕМОНСТРАЦИЯ ОПЕРАТОРА << ДЛЯ PATH ==========
    Path bfsPath = bfs.findPath(n0, n5);
    cout << "Путь от 0 до 5 (BFS): " << bfsPath << endl;
    cout << "Расстояние: " << bfs.findDistance(n0, n5) << endl;
    cout << endl;

    // ========== DFS ==========
    cout << "=== ПОИСК В ГЛУБИНУ (DFS) ===" << endl;
    cout << "----------------------------------------" << endl;

    DFS dfs(graph);

    cout << "Существует ли путь между 0 и 5? "
        << (dfs.connected(n0, n5) ? "Да" : "Нет") << endl;

    cout << "Существует ли путь между 0 и 2? "
        << (dfs.connected(n0, n2) ? "Да" : "Нет") << endl;

    // ========== ДЕМОНСТРАЦИЯ ОПЕРАТОРА [] ДЛЯ PATH ==========
    Path dfsPath = dfs.findPath(n0, n5);
    cout << "Путь от 0 до 5 (DFS): " << dfsPath << endl;

    // Демонстрация operator[]
    cout << "Первый элемент пути: " << *dfsPath[0] << endl;
    cout << endl;

    // ========== ДЕМОНСТРАЦИЯ OPERATOR BOOL ДЛЯ PATH ==========
    cout << "=== ПРОВЕРКА ПУТИ (operator bool) ===" << endl;
    Path emptyPath;
    if (bfsPath) {
        cout << "BFS путь найден" << endl;
    }
    if (!emptyPath) {
        cout << "Пустой путь" << endl;
    }
    cout << endl;

    // ========== СРАВНЕНИЕ BFS И DFS ==========
    cout << "=== СРАВНЕНИЕ BFS И DFS ===" << endl;
    cout << "----------------------------------------" << endl;
    cout << "BFS находит кратчайший путь (по количеству рёбер)" << endl;
    cout << "DFS находит путь, но не обязательно кратчайший" << endl;
    cout << endl;
    cout << "BFS использует очередь (FIFO) - расширяет поиск волнами" << endl;
    cout << "DFS использует стек (рекурсию) - идёт вглубь до упора" << endl;

    // ========== ОЧИСТКА ПАМЯТИ ==========
    // Внимание! Вершины удаляются в деструкторе Graph.
    // Не нужно удалять их здесь, иначе будет double delete.

    return 0;
}