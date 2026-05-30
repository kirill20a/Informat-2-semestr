#ifndef GRAPH_H
#define GRAPH_H

#include <iostream>
#include <string>
#include <set>
#include <queue>
#include <map>
#include <vector>
#include <fstream>

using namespace std;

typedef set<class Node*>::const_iterator node_iterator;

// ===== ВЕРШИНА =====
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

    // Операторы сравнения
    bool operator==(const Node& other) const;
    bool operator!=(const Node& other) const;
    bool operator<(const Node& other) const;
    Node& operator=(const Node& other);

    friend ostream& operator<<(ostream& out, const Node& node);
    friend class Graph;
    friend class BFS;
    friend class DFS;
};

// ===== ГРАФ =====
class Graph {
private:
    set<Node*> nodes;

    // Ребро с весом
    struct Edge {
        Node* from;
        Node* to;
        int weight;
        Edge(Node* f, Node* t, int w) : from(f), to(t), weight(w) {}
        bool operator<(const Edge& other) const {
            if (from != other.from) return from < other.from;
            return to < other.to;
        }
        bool operator==(const Edge& other) const {
            return (from == other.from && to == other.to) ||
                (from == other.to && to == other.from);
        }
        bool operator!=(const Edge& other) const {
            return !(*this == other);
        }
    };
    set<Edge> edges;

public:
    Graph();
    ~Graph();

    // ========== КОНСТРУКТОР КОПИРОВАНИЯ ==========
    Graph(const Graph& other);

    // ========== ОПЕРАТОР ПРИСВАИВАНИЯ ==========
    Graph& operator=(const Graph& other);

    // Конструктор из файла (формат: Source Target)
    Graph(const string& filename);

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

    // Новые методы для задания
    vector<Graph> getConnectedComponents() const;
    void saveToFile(const string& filename) const;
    void saveConnectedComponents() const;

    friend ostream& operator<<(ostream& out, const Graph& graph);
};

// ===== ПУТЬ =====
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

// ===== ДЕЙКСТРА =====
struct Way {
    vector<Node*> nodes;
    int length;
    Way() : length(-1) {}
};

class Dijkstra {
private:
    const Graph& graph;

    struct MarkedNode {
        Node* node;
        int mark;
        Node* prev;
        MarkedNode(Node* anode = nullptr, int amark = 0, Node* aprev = nullptr)
            : node(anode), mark(amark), prev(aprev) {
        }
    };

    class PriorityQueue {
    private:
        vector<MarkedNode> nodes;
    public:
        MarkedNode pop();
        void push(Node* node, int mark, Node* prev);
        bool empty() const { return nodes.empty(); }
    };

    static Way unroll(map<Node*, MarkedNode>& visited, Node* begin, Node* curr);

public:
    Dijkstra(const Graph& agraph) : graph(agraph) {}
    Way shortestWay(Node* begin, Node* end);
};

#endif
