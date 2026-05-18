#ifndef GRAPH_H
#define GRAPH_H

#include <iostream>
#include <string>
#include <set>
#include <queue>

using namespace std;

typedef set<class Node*>::const_iterator node_iterator;

// ===== Вершина графа =====
// У вершины есть имя и множество соседей (указатели)
// Соседи хранятся в set → уникальность и быстрый поиск
class Node {
private:
    string name;
    set<Node*> neighbours;

    void addNeighbour(Node* neighbour);
    void removeNeighbour(Node* neighbour);

public:
    Node(const string& aname);
    ~Node();

    const string& getName() const;

    // итераторы для перебора соседей
    node_iterator nb_begin() const;
    node_iterator nb_end() const;

    // сравнение и присваивание (по имени)
    bool operator==(const Node& other) const;
    bool operator!=(const Node& other) const;
    bool operator<(const Node& other) const;
    Node& operator=(const Node& other);

    friend ostream& operator<<(ostream& out, const Node& node);

    // доступ к приватным полям для управления графом
    friend class Graph;
    friend class BFS;
    friend class DFS;
};

// ===== Граф (множество вершин + рёбер) =====
class Graph {
private:
    set<Node*> nodes;   // все вершины графа

    // ребро: from → to + вес
    struct Edge {
        Node* from;
        Node* to;
        int weight;
        Edge(Node* f, Node* t, int w) : from(f), to(t), weight(w) {}
        bool operator<(const Edge& other) const;
        bool operator==(const Edge& other) const;
        bool operator!=(const Edge& other) const;
    };
    set<Edge> edges;   // все рёбра (с весами)

public:
    Graph();
    ~Graph();

    // копирование запрещено (указатели сломаются)
    Graph(const Graph& other) = delete;
    Graph& operator=(const Graph& other) = delete;

    void addNode(Node* node);
    void removeNode(Node* node);
    void addEdge(Node* begin, Node* end, int weight = 1);
    void removeEdge(Node* begin, Node* end);

    bool hasNode(Node* node) const;
    bool hasEdge(Node* begin, Node* end) const;
    int getWeight(Node* begin, Node* end) const;

    node_iterator begin() const;
    node_iterator end() const;
    size_t getNodeCount() const;

    friend ostream& operator<<(ostream& out, const Graph& graph);
};

// ===== Путь (последовательность вершин) =====
class Path {
private:
    Node* nodes[1000];   // максимум 1000 вершин
    int len;

public:
    Path() : len(0) {}

    Path(const Path& other);
    Path& operator=(const Path& other);

    Node* operator[](int i) const;
    explicit operator bool() const;   // можно проверить if (path)

    void pushBack(Node* node);
    void popBack();
    Node* get(int i) const;
    int getLength() const;
    void clear();
    bool empty() const;
    void print(ostream& out = cout) const;

    friend ostream& operator<<(ostream& out, const Path& path);
};

ostream& operator<<(ostream& out, const Path& path);

// ===== BFS — обход в ширину (очередь) =====
// Используется для поиска кратчайшего пути по числу рёбер
class BFS {
private:
    const Graph& graph;   // работаем с чужим графом (не копируем)

public:
    BFS(const Graph& agraph);
    BFS& operator=(const BFS& other) = delete;

    bool connected(Node* begin, Node* end);      // есть ли путь?
    Path findPath(Node* begin, Node* end);       // восстановить путь
    int findDistance(Node* begin, Node* end);    // сколько рёбер
};

// ===== DFS — обход в глубину (рекурсия) =====
// Идёт вглубь, возвращается — не гарантирует кратчайший путь
class DFS {
private:
    const Graph& graph;
    set<Node*> visited;   // чтобы не ходить по кругу

    bool connectedRecursive(Node* begin, Node* end, int depth);
    bool findPathRecursive(Node* current, Node* end, Path& path, set<Node*>& visitedNodes);

public:
    DFS(const Graph& agraph);
    DFS& operator=(const DFS& other) = delete;

    bool connected(Node* begin, Node* end);
    Path findPath(Node* begin, Node* end);
};

#endif