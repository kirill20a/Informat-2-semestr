#ifndef GRAPH_H
#define GRAPH_H

#include <iostream>
#include <string>
#include <set>
#include <queue>

using namespace std;

typedef set<class Node*>::const_iterator node_iterator;

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

    // ========== ÎÏÅĞÀÒÎĞÛ ÑĞÀÂÍÅÍÈß ==========
    bool operator==(const Node& other) const;
    bool operator!=(const Node& other) const;
    bool operator<(const Node& other) const;

    // ========== ÎÏÅĞÀÒÎĞ ÏĞÈÑÂÀÈÂÀÍÈß ==========
    Node& operator=(const Node& other);

    // ========== ÎÏÅĞÀÒÎĞ ÂÛÂÎÄÀ ==========
    friend ostream& operator<<(ostream& out, const Node& node);

    friend class Graph;
    friend class BFS;
    friend class DFS;
};

class Graph {
private:
    set<Node*> nodes;

    // Ñòğóêòóğà äëÿ õğàíåíèÿ ğåáğà ñ âåñîì
    struct Edge {
        Node* from;
        Node* to;
        int weight;
        Edge(Node* f, Node* t, int w) : from(f), to(t), weight(w) {}
        bool operator<(const Edge& other) const {
            if (from != other.from) return from < other.from;
            return to < other.to;
        }
        // ========== ÎÏÅĞÀÒÎĞÛ ÑĞÀÂÍÅÍÈß ÄËß EDGE ==========
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

    // ========== ÎÏÅĞÀÒÎĞ ÏĞÈÑÂÀÈÂÀÍÈß (ÇÀÏĞÅÙ¨Í) ==========
    Graph& operator=(const Graph& other) = delete;

    // ========== ÎÏÅĞÀÒÎĞ ÊÎÏÈĞÎÂÀÍÈß (ÇÀÏĞÅÙ¨Í) ==========
    Graph(const Graph& other) = delete;

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

    // ========== ÎÏÅĞÀÒÎĞ ÂÛÂÎÄÀ ÃĞÀÔÀ ==========
    friend ostream& operator<<(ostream& out, const Graph& graph);
};

// ========== ÊËÀÑÑ PATH ==========
class Path {
private:
    Node* nodes[1000];
    int len;

public:
    Path() : len(0) {}

    // ========== ÎÏÅĞÀÒÎĞ ÊÎÏÈĞÎÂÀÍÈß ==========
    Path(const Path& other) {
        len = other.len;
        for (int i = 0; i < len; i++) {
            nodes[i] = other.nodes[i];
        }
    }

    // ========== ÎÏÅĞÀÒÎĞ ÏĞÈÑÂÀÈÂÀÍÈß ==========
    Path& operator=(const Path& other) {
        if (this != &other) {
            len = other.len;
            for (int i = 0; i < len; i++) {
                nodes[i] = other.nodes[i];
            }
        }
        return *this;
    }

    // ========== ÎÏÅĞÀÒÎĞ ÈÍÄÅÊÑÀÖÈÈ ==========
    Node* operator[](int i) const {
        return (i >= 0 && i < len) ? nodes[i] : nullptr;
    }

    // ========== ÎÏÅĞÀÒÎĞ ÏĞÈÂÅÄÅÍÈß Ê BOOL ==========
    explicit operator bool() const {
        return len > 0;
    }

    void pushBack(Node* node) {
        if (len < 1000) nodes[len++] = node;
    }

    void popBack() {
        if (len > 0) len--;
    }

    Node* get(int i) const {
        return (i >= 0 && i < len) ? nodes[i] : nullptr;
    }

    int getLength() const {
        return len;
    }

    void clear() {
        len = 0;
    }

    bool empty() const {
        return len == 0;
    }

    void print(ostream& out = cout) const {
        for (int i = 0; i < len; i++) {
            out << nodes[i]->getName();
            if (i < len - 1) out << " -> ";
        }
    }

    // ========== ÎÏÅĞÀÒÎĞ ÂÛÂÎÄÀ ==========
    friend ostream& operator<<(ostream& out, const Path& path);
};

// ========== ÎÏÅĞÀÒÎĞ ÂÛÂÎÄÀ ÄËß PATH ==========
ostream& operator<<(ostream& out, const Path& path);

class BFS {
private:
    const Graph& graph;

public:
    BFS(const Graph& agraph);

    // ========== ÎÏÅĞÀÒÎĞ ÏĞÈÑÂÀÈÂÀÍÈß (ÇÀÏĞÅÙ¨Í) ==========
    BFS& operator=(const BFS& other) = delete;

    bool connected(Node* begin, Node* end);
    Path findPath(Node* begin, Node* end);
    int findDistance(Node* begin, Node* end);
};

class DFS {
private:
    const Graph& graph;
    set<Node*> visited;

    bool connectedRecursive(Node* begin, Node* end, int depth);
    bool findPathRecursive(Node* current, Node* end, Path& path, set<Node*>& visitedNodes);

public:
    DFS(const Graph& agraph);

    // ========== ÎÏÅĞÀÒÎĞ ÏĞÈÑÂÀÈÂÀÍÈß (ÇÀÏĞÅÙ¨Í) ==========
    DFS& operator=(const DFS& other) = delete;

    bool connected(Node* begin, Node* end);
    Path findPath(Node* begin, Node* end);
};

#endif // GRAPH_H