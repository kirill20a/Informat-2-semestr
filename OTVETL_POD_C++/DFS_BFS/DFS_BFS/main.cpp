#include "Graph.h"
#include <iostream>

using namespace std;

int main() {
    setlocale(LC_ALL, "Russian");

    // ===== 1. СОЗДАНИЕ ГРАФА =====
    Graph graph;

    Node* n0 = new Node("0");
    Node* n1 = new Node("1");
    Node* n2 = new Node("2");
    Node* n3 = new Node("3");
    Node* n4 = new Node("4");
    Node* n5 = new Node("5");

    graph.addNode(n0);
    graph.addNode(n1);
    graph.addNode(n2);
    graph.addNode(n3);
    graph.addNode(n4);
    graph.addNode(n5);

    graph.addEdge(n0, n1, 10);
    graph.addEdge(n1, n2, 15);
    graph.addEdge(n2, n5, 30);
    graph.addEdge(n0, n5, 50);
    graph.addEdge(n0, n3, 20);
    graph.addEdge(n3, n4, 25);
    graph.addEdge(n4, n5, 15);
    graph.addEdge(n1, n3, 10);
    graph.addEdge(n2, n4, 5);

    cout << graph << endl;

    // ===== 2. BFS =====
    BFS bfs(graph);
    cout << "BFS: путь между 0 и 5? " << bfs.connected(n0, n5) << endl;
    cout << "Расстояние (рёбер): " << bfs.findDistance(n0, n5) << endl;

    // ===== 3. DFS =====
    DFS dfs(graph);
    cout << "DFS путь между 0 и 5: " << dfs.findPath(n0, n5) << endl;

    // ===== 4. ДЕЙКСТРА =====
    Dijkstra dijkstra(graph);
    Way way = dijkstra.shortestWay(n0, n5);
    if (way.length != -1) {
        cout << "Дейкстра кратчайший путь: ";
        for (Node* node : way.nodes) cout << node->getName() << " ";
        cout << "\nСуммарный вес: " << way.length << endl;
    }

    // ===== 5. КОМПОНЕНТЫ СВЯЗНОСТИ (из файла) — НОВАЯ ВЕРСИЯ БЕЗ КОПИРОВАНИЯ =====
    Graph bigGraph("1000.csv");
    cout << "\nГраф загружен, вершин: " << bigGraph.getNodeCount() << endl;

    // ВЫЗЫВАЕМ НОВУЮ ФУНКЦИЮ (БЕЗ КОПИРОВАНИЯ)
    bigGraph.saveConnectedComponents();

    return 0;
}
