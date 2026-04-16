#include "Graph.h"
#include <algorithm>

using namespace std;

// ========== ĞÅÀËÈÇÀÖÈß NODE ==========

Node::Node(const string& aname) : name(aname) {}
Node::~Node() {}

const string& Node::getName() const { return name; }

void Node::addNeighbour(Node* neighbour) {
    if (neighbour) neighbours.insert(neighbour);
}

void Node::removeNeighbour(Node* neighbour) {
    if (neighbour) neighbours.erase(neighbour);
}

node_iterator Node::nb_begin() const { return neighbours.begin(); }
node_iterator Node::nb_end() const { return neighbours.end(); }

bool Node::operator==(const Node& other) const { return name == other.name; }
bool Node::operator!=(const Node& other) const { return !(*this == other); }
bool Node::operator<(const Node& other) const { return name < other.name; }

// ========== ĞÅÀËÈÇÀÖÈß GRAPH ==========

Graph::Graph() {}
Graph::~Graph() {
    for (Node* node : nodes) delete node;
}

void Graph::addNode(Node* node) {
    if (node) nodes.insert(node);
}

void Graph::removeNode(Node* node) {
    if (!node) return;
    nodes.erase(node);
    for (Node* other : nodes) other->removeNeighbour(node);
    for (auto it = edges.begin(); it != edges.end();) {
        if (it->from == node || it->to == node) it = edges.erase(it);
        else ++it;
    }
    delete node;
}

void Graph::addEdge(Node* begin, Node* end, int weight) {
    if (!begin || !end) return;
    if (nodes.find(begin) == nodes.end()) return;
    if (nodes.find(end) == nodes.end()) return;

    begin->addNeighbour(end);
    end->addNeighbour(begin);
    edges.insert(Edge(begin, end, weight));
}

void Graph::removeEdge(Node* begin, Node* end) {
    if (!begin || !end) return;
    begin->removeNeighbour(end);
    end->removeNeighbour(begin);

    for (auto it = edges.begin(); it != edges.end(); ++it) {
        if ((it->from == begin && it->to == end) ||
            (it->from == end && it->to == begin)) {
            edges.erase(it);
            break;
        }
    }
}

bool Graph::hasNode(Node* node) const { return nodes.find(node) != nodes.end(); }
bool Graph::hasEdge(Node* begin, Node* end) const { return getWeight(begin, end) != 0; }

int Graph::getWeight(Node* begin, Node* end) const {
    for (const Edge& e : edges) {
        if ((e.from == begin && e.to == end) || (e.from == end && e.to == begin)) {
            return e.weight;
        }
    }
    return 0;
}

node_iterator Graph::begin() const { return nodes.begin(); }
node_iterator Graph::end() const { return nodes.end(); }
size_t Graph::getNodeCount() const { return nodes.size(); }

// ========== ÎÏÅĞÀÒÎĞ ÂÛÂÎÄÀ ÃĞÀÔÀ ==========

ostream& operator<<(ostream& out, const Graph& graph) {
    out << "Ãğàô:" << endl;
    out << "Êîëè÷åñòâî âåğøèí: " << graph.getNodeCount() << endl;
    for (Node* node : graph) {
        out << "  Âåğøèíà " << node->getName() << " -> ñîñåäè: ";
        for (node_iterator it = node->nb_begin(); it != node->nb_end(); ++it) {
            int w = graph.getWeight(node, *it);
            out << (*it)->getName();
            if (w != 1) out << "(" << w << ")";
            out << " ";
        }
        out << endl;
    }
    return out;
}

// ========== ÎÏÅĞÀÒÎĞ ÂÛÂÎÄÀ ÏÓÒÈ ==========

ostream& operator<<(ostream& out, const Path& path) {
    if (path.getLength() == 0) {
        out << "ïóòü íå íàéäåí";
        return out;
    }
    for (int i = 0; i < path.getLength(); i++) {
        out << path.get(i)->getName();
        if (i < path.getLength() - 1) out << " -> ";
    }
    return out;
}

// ========== ĞÅÀËÈÇÀÖÈß BFS ==========

BFS::BFS(const Graph& agraph) : graph(agraph) {}

bool BFS::connected(Node* begin, Node* end) {
    if (!begin || !end) return false;
    if (begin == end) return true;

    queue<Node*> q;
    set<Node*> visited;

    q.push(begin);
    visited.insert(begin);

    while (!q.empty()) {
        Node* current = q.front();
        q.pop();

        for (node_iterator it = current->nb_begin(); it != current->nb_end(); ++it) {
            Node* neighbour = *it;
            if (neighbour == end) return true;
            if (visited.find(neighbour) == visited.end()) {
                visited.insert(neighbour);
                q.push(neighbour);
            }
        }
    }
    return false;
}

Path BFS::findPath(Node* begin, Node* end) {
    Path path;
    if (!begin || !end) return path;
    if (begin == end) {
        path.pushBack(begin);
        return path;
    }
    return path;
}

int BFS::findDistance(Node* begin, Node* end) {
    if (!begin || !end) return -1;
    if (begin == end) return 0;

    queue<Node*> q;
    set<Node*> visited;

    q.push(begin);
    visited.insert(begin);

    int distance = 0;
    while (!q.empty()) {
        int levelSize = q.size();
        distance++;
        for (int i = 0; i < levelSize; i++) {
            Node* current = q.front();
            q.pop();
            for (node_iterator it = current->nb_begin(); it != current->nb_end(); ++it) {
                Node* neighbour = *it;
                if (neighbour == end) return distance;
                if (visited.find(neighbour) == visited.end()) {
                    visited.insert(neighbour);
                    q.push(neighbour);
                }
            }
        }
    }
    return -1;
}

// ========== ĞÅÀËÈÇÀÖÈß DFS ==========

DFS::DFS(const Graph& agraph) : graph(agraph) {}

bool DFS::connectedRecursive(Node* begin, Node* end, int depth) {
    (void)depth;
    if (begin == end) return true;

    visited.insert(begin);

    for (node_iterator it = begin->nb_begin(); it != begin->nb_end(); ++it) {
        Node* neighbour = *it;
        if (visited.find(neighbour) == visited.end()) {
            if (connectedRecursive(neighbour, end, depth + 1)) return true;
        }
    }
    return false;
}

bool DFS::connected(Node* begin, Node* end) {
    visited.clear();
    return connectedRecursive(begin, end, 0);
}

bool DFS::findPathRecursive(Node* current, Node* end, Path& path, set<Node*>& visitedNodes) {
    path.pushBack(current);

    if (current == end) return true;

    visitedNodes.insert(current);

    for (node_iterator it = current->nb_begin(); it != current->nb_end(); ++it) {
        Node* neighbour = *it;
        if (visitedNodes.find(neighbour) == visitedNodes.end()) {
            if (findPathRecursive(neighbour, end, path, visitedNodes)) {
                return true;
            }
        }
    }

    path.popBack();
    return false;
}

Path DFS::findPath(Node* begin, Node* end) {
    Path path;
    set<Node*> visitedNodes;
    if (!begin || !end) return path;
    findPathRecursive(begin, end, path, visitedNodes);
    return path;
}
// ========== ÎÏÅĞÀÒÎĞ ÏĞÈÑÂÀÈÂÀÍÈß ÄËß NODE ==========
Node& Node::operator=(const Node& other) {
    if (this != &other) {
        name = other.name;
        // neighbours íå êîïèğóåì, òàê êàê ıòî íàğóøèò öåëîñòíîñòü ãğàôà
    }
    return *this;
}

// ========== ÎÏÅĞÀÒÎĞ ÂÛÂÎÄÀ ÄËß NODE ==========
ostream& operator<<(ostream& out, const Node& node) {
    out << node.getName();
    return out;
}