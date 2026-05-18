#include "Graph.h"
#include <queue>
#include <set>

using namespace std;

// ---------- Node ----------
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

Node& Node::operator=(const Node& other) {
    if (this != &other) name = other.name;
    return *this;
}

ostream& operator<<(ostream& out, const Node& node) {
    out << node.getName();
    return out;
}

// ---------- Graph ----------
Graph::Graph() {}

Graph::~Graph() {
    for (Node* node : nodes) delete node;
}

Graph::Graph(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) return;

    string line;
    getline(file, line); // пропускаем заголовок "Source Target"

    int from, to;
    while (file >> from >> to) {
        Node* nodeFrom = nullptr;
        Node* nodeTo = nullptr;

        for (Node* n : nodes) {
            if (n->getName() == to_string(from)) nodeFrom = n;
            if (n->getName() == to_string(to)) nodeTo = n;
        }

        if (!nodeFrom) {
            nodeFrom = new Node(to_string(from));
            addNode(nodeFrom);
        }
        if (!nodeTo) {
            nodeTo = new Node(to_string(to));
            addNode(nodeTo);
        }

        addEdge(nodeFrom, nodeTo);
    }
    file.close();
}

void Graph::addNode(Node* node) {
    if (node) nodes.insert(node);
}

void Graph::removeNode(Node* node) {
    if (!node) return;
    nodes.erase(node);
    for (Node* other : nodes) other->removeNeighbour(node);
    delete node;
}

void Graph::addEdge(Node* begin, Node* end) {
    if (!begin || !end) return;
    if (!hasNode(begin) || !hasNode(end)) return;
    begin->addNeighbour(end);
    end->addNeighbour(begin);
    edges.insert({ begin, end });
}

bool Graph::hasNode(Node* node) const {
    return nodes.find(node) != nodes.end();
}

bool Graph::hasEdge(Node* begin, Node* end) const {
    return edges.find({ begin, end }) != edges.end() ||
        edges.find({ end, begin }) != edges.end();
}

node_iterator Graph::begin() const { return nodes.begin(); }
node_iterator Graph::end() const { return nodes.end(); }
size_t Graph::getNodeCount() const { return nodes.size(); }

vector<vector<Node*>> Graph::findConnectedComponents() {
    vector<vector<Node*>> components;
    set<Node*> visited;

    for (Node* start : nodes) {
        if (visited.find(start) != visited.end()) continue;

        // BFS для новой компоненты
        vector<Node*> component;
        queue<Node*> q;
        q.push(start);
        visited.insert(start);

        while (!q.empty()) {
            Node* current = q.front(); q.pop();
            component.push_back(current);
            for (node_iterator it = current->nb_begin(); it != current->nb_end(); ++it) {
                if (visited.find(*it) == visited.end()) {
                    visited.insert(*it);
                    q.push(*it);
                }
            }
        }
        components.push_back(component);
    }
    return components;
}

void Graph::saveComponent(const vector<Node*>& component, const string& filename) const {
    ofstream file(filename);
    set<pair<int, int>> edgesWritten;

    for (Node* node : component) {
        int from = stoi(node->getName());
        for (node_iterator it = node->nb_begin(); it != node->nb_end(); ++it) {
            int to = stoi((*it)->getName());
            if (edgesWritten.find({ from, to }) == edgesWritten.end() &&
                edgesWritten.find({ to, from }) == edgesWritten.end()) {
                file << from << "\t" << to << endl;
                edgesWritten.insert({ from, to });
            }
        }
    }
    file.close();
}

ostream& operator<<(ostream& out, const Graph& graph) {
    out << "Граф:\n  Вершин: " << graph.getNodeCount() << "\n";
    for (Node* node : graph) {
        out << "  " << node->getName() << " -> ";
        for (auto it = node->nb_begin(); it != node->nb_end(); ++it)
            out << (*it)->getName() << " ";
        out << "\n";
    }
    return out;
}

// ---------- Path ----------
Path::Path(const Path& other) : len(other.len) {
    for (int i = 0; i < len; ++i) nodes[i] = other.nodes[i];
}

Path& Path::operator=(const Path& other) {
    if (this != &other) {
        len = other.len;
        for (int i = 0; i < len; ++i) nodes[i] = other.nodes[i];
    }
    return *this;
}

Node* Path::operator[](int i) const {
    return (i >= 0 && i < len) ? nodes[i] : nullptr;
}

Path::operator bool() const { return len > 0; }

void Path::pushBack(Node* node) {
    if (len < 1000) nodes[len++] = node;
}

void Path::popBack() { if (len > 0) --len; }

Node* Path::get(int i) const { return (*this)[i]; }
int Path::getLength() const { return len; }
void Path::clear() { len = 0; }
bool Path::empty() const { return len == 0; }

ostream& operator<<(ostream& out, const Path& path) {
    for (int i = 0; i < path.getLength(); ++i) {
        out << path.get(i)->getName();
        if (i < path.getLength() - 1) out << " -> ";
    }
    return out;
}

// ---------- BFS ----------
BFS::BFS(const Graph& agraph) : graph(agraph) {}

bool BFS::connected(Node* begin, Node* end) {
    if (!begin || !end) return false;
    if (begin == end) return true;

    queue<Node*> q;
    set<Node*> visited;
    q.push(begin);
    visited.insert(begin);

    while (!q.empty()) {
        Node* cur = q.front(); q.pop();
        for (auto it = cur->nb_begin(); it != cur->nb_end(); ++it) {
            if (*it == end) return true;
            if (!visited.count(*it)) {
                visited.insert(*it);
                q.push(*it);
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
    // Упрощённо: возвращаем пустой путь. Для реального пути нужен map предков.
    return path;
}

int BFS::findDistance(Node* begin, Node* end) {
    if (!begin || !end) return -1;
    if (begin == end) return 0;

    queue<Node*> q;
    set<Node*> visited;
    q.push(begin);
    visited.insert(begin);

    int dist = 0;
    while (!q.empty()) {
        int sz = q.size();
        dist++;
        for (int i = 0; i < sz; ++i) {
            Node* cur = q.front(); q.pop();
            for (auto it = cur->nb_begin(); it != cur->nb_end(); ++it) {
                if (*it == end) return dist;
                if (!visited.count(*it)) {
                    visited.insert(*it);
                    q.push(*it);
                }
            }
        }
    }
    return -1;
}

// ---------- DFS ----------
DFS::DFS(const Graph& agraph) : graph(agraph) {}

bool DFS::connectedRecursive(Node* begin, Node* end, int) {
    if (begin == end) return true;
    visited.insert(begin);
    for (auto it = begin->nb_begin(); it != begin->nb_end(); ++it) {
        if (!visited.count(*it) && connectedRecursive(*it, end, 0))
            return true;
    }
    return false;
}

bool DFS::connected(Node* begin, Node* end) {
    visited.clear();
    return connectedRecursive(begin, end, 0);
}

bool DFS::findPathRecursive(Node* cur, Node* end, Path& path, set<Node*>& seen) {
    path.pushBack(cur);
    if (cur == end) return true;
    seen.insert(cur);
    for (auto it = cur->nb_begin(); it != cur->nb_end(); ++it) {
        if (!seen.count(*it) && findPathRecursive(*it, end, path, seen))
            return true;
    }
    path.popBack();
    return false;
}

Path DFS::findPath(Node* begin, Node* end) {
    Path path;
    set<Node*> seen;
    if (begin && end) findPathRecursive(begin, end, path, seen);
    return path;
}