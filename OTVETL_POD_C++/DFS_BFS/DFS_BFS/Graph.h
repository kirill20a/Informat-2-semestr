#ifndef GRAPH_H
#define GRAPH_H

#include <iostream>
#include <string>
#include <set>
#include <queue>
#include <vector>
#include <map>
#include <fstream>
#include <algorithm>

using namespace std;

typedef set<class Node*>::const_iterator node_iterator;

// ===== Вершина графа =====
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

    node_iterator nb_begin() const;
    node_iterator nb_end() const;

    bool operator==(const Node& other) const;
    bool operator!=(const Node& other) const;
    bool operator<(const Node& other) const;
    Node& operator=(const Node& other);

    friend ostream& operator<<(ostream& out, const Node& node);
    friend class Graph;
    friend class BFS;
    friend class DFS;
};

// ===== Граф =====
class Graph {
private:
    set<Node*> nodes;
    set<pair<Node*, Node*>> edges;   // для хранения рёбер (упрощённо)

public:
    Graph();
    ~Graph();

    // Запрет копирования
    Graph(const Graph& other) = delete;
    Graph& operator=(const Graph& other) = delete;

    // Конструктор из файла (формат: Source Target)
    Graph(const string& filename);

    void addNode(Node* node);
    void removeNode(Node* node);
    void addEdge(Node* begin, Node* end);

    bool hasNode(Node* node) const;
    bool hasEdge(Node* begin, Node* end) const;

    node_iterator begin() const;
    node_iterator end() const;
    size_t getNodeCount() const;

    // Поиск компонент связности (непересекающихся графов)
    vector<vector<Node*>> findConnectedComponents();

    // Сохранить компоненту в файл (формат как у TestGraph.txt)
    void saveComponent(const vector<Node*>& component, const string& filename) const;

    friend ostream& operator<<(ostream& out, const Graph& graph);
};

// ===== Путь (для BFS/DFS) =====
class Path {
private:
    Node* nodes[1000];
    int len;

public:
    Path() : len(0) {}
    Path(const Path& other);
    Path& operator=(const Path& other);

    Node* operator[](int i) const;
    explicit operator bool() const;

    void pushBack(Node* node);
    void popBack();
    Node* get(int i) const;
    int getLength() const;
    void clear();
    bool empty() const;

    friend ostream& operator<<(ostream& out, const Path& path);
};

ostream& operator<<(ostream& out, const Path& path);

// ===== BFS =====
class BFS {
private:
    const Graph& graph;

public:
    BFS(const Graph& agraph);
    BFS& operator=(const BFS& other) = delete;

    bool connected(Node* begin, Node* end);
    Path findPath(Node* begin, Node* end);
    int findDistance(Node* begin, Node* end);
};

// ===== DFS =====
class DFS {
private:
    const Graph& graph;
    set<Node*> visited;

    bool connectedRecursive(Node* begin, Node* end, int depth);
    bool findPathRecursive(Node* current, Node* end, Path& path, set<Node*>& visitedNodes);

public:
    DFS(const Graph& agraph);
    DFS& operator=(const DFS& other) = delete;

    bool connected(Node* begin, Node* end);
    Path findPath(Node* begin, Node* end);
};

#endif