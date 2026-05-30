#include "Graph.h"
#include <algorithm>

using namespace std;

// ========== NODE ==========

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

// ========== GRAPH ==========

Graph::Graph() {}

Graph::~Graph() {
    for (Node* node : nodes) delete node;
}
// ========== КОНСТРУКТОР КОПИРОВАНИЯ ==========
Graph::Graph(const Graph& other) {
    map<const Node*, Node*> oldToNew;

    // Копируем вершины
    for (Node* oldNode : other.nodes) {
        Node* newNode = new Node(oldNode->getName());
        nodes.insert(newNode);
        oldToNew[oldNode] = newNode;
    }

    // Копируем рёбра
    for (const Edge& e : other.edges) {
        Node* newFrom = oldToNew[e.from];
        Node* newTo = oldToNew[e.to];
        addEdge(newFrom, newTo, e.weight);
    }
}

// ========== ОПЕРАТОР ПРИСВАИВАНИЯ ==========
Graph& Graph::operator=(const Graph& other) {
    if (this != &other) {
        // Очищаем текущий граф
        for (Node* node : nodes) delete node;
        nodes.clear();
        edges.clear();

        // Копируем вершины
        map<const Node*, Node*> oldToNew;
        for (Node* oldNode : other.nodes) {
            Node* newNode = new Node(oldNode->getName());
            nodes.insert(newNode);
            oldToNew[oldNode] = newNode;
        }

        // Копируем рёбра
        for (const Edge& e : other.edges) {
            Node* newFrom = oldToNew[e.from];
            Node* newTo = oldToNew[e.to];
            addEdge(newFrom, newTo, e.weight);
        }
    }
    return *this;
}
// Конструктор из файла
Graph::Graph(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) return;

    string line;
    getline(file, line); // пропускаем заголовок

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

// ========== НОВЫЕ МЕТОДЫ ==========

vector<Graph> Graph::getConnectedComponents() const {
    vector<Graph> components;
    set<Node*> visited;

    for (Node* start : nodes) {
        if (visited.find(start) != visited.end()) continue;

        Graph component;
        queue<Node*> q;
        q.push(start);
        visited.insert(start);
        component.addNode(start);

        while (!q.empty()) {
            Node* current = q.front();
            q.pop();

            for (node_iterator it = current->nb_begin(); it != current->nb_end(); ++it) {
                Node* neighbour = *it;
                component.addNode(neighbour);
                component.addEdge(current, neighbour, getWeight(current, neighbour));

                if (visited.find(neighbour) == visited.end()) {
                    visited.insert(neighbour);
                    q.push(neighbour);
                }
            }
        }
        components.push_back(component);
    }
    return components;
}

void Graph::saveToFile(const string& filename) const {
    ofstream file(filename);
    if (!file.is_open()) return;

    file << "Source\tTarget" << endl;

    set<pair<Node*, Node*>> writtenEdges;

    for (Node* node : nodes) {
        for (node_iterator it = node->nb_begin(); it != node->nb_end(); ++it) {
            Node* neighbour = *it;
            if (writtenEdges.find({ node, neighbour }) == writtenEdges.end() &&
                writtenEdges.find({ neighbour, node }) == writtenEdges.end()) {
                file << node->getName() << "\t" << neighbour->getName() << endl;
                writtenEdges.insert({ node, neighbour });
            }
        }
    }
    file.close();
}
void Graph::saveConnectedComponents() const {
    set<Node*> visited;
    int compNum = 1;

    for (Node* start : nodes) {
        if (visited.find(start) != visited.end()) continue;

        string filename = "component_" + to_string(compNum) + ".txt";
        ofstream file(filename);
        if (!file.is_open()) {
            cerr << "Не удалось создать файл " << filename << endl;
            return;
        }

        file << "Source\tTarget" << endl;

        queue<Node*> q;
        q.push(start);
        visited.insert(start);

        set<pair<Node*, Node*>> writtenEdges;

        while (!q.empty()) {
            Node* current = q.front();
            q.pop();

            for (node_iterator it = current->nb_begin(); it != current->nb_end(); ++it) {
                Node* neighbour = *it;

                if (writtenEdges.find({ current, neighbour }) == writtenEdges.end() &&
                    writtenEdges.find({ neighbour, current }) == writtenEdges.end()) {
                    file << current->getName() << "\t" << neighbour->getName() << endl;
                    writtenEdges.insert({ current, neighbour });
                }

                if (visited.find(neighbour) == visited.end()) {
                    visited.insert(neighbour);
                    q.push(neighbour);
                }
            }
        }

        file.close();
        cout << "Компонента " << compNum << ": сохранена в " << filename << endl;
        compNum++;
    }
}
ostream& operator<<(ostream& out, const Graph& graph) {
    out << "Граф:\n  Вершин: " << graph.getNodeCount() << "\n";
    for (Node* node : graph) {
        out << "  " << node->getName() << " -> ";
        for (node_iterator it = node->nb_begin(); it != node->nb_end(); ++it)
            out << (*it)->getName() << " ";
        out << "\n";
    }
    return out;
}

// ========== PATH ==========

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

// ========== BFS ==========

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
    if (begin == end) path.pushBack(begin);
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

// ========== DFS ==========

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

// ========== ДЕЙКСТРА ==========

Dijkstra::MarkedNode Dijkstra::PriorityQueue::pop() {
    MarkedNode mn = nodes.back();
    nodes.pop_back();
    return mn;
}

void Dijkstra::PriorityQueue::push(Node* node, int mark, Node* prev) {
    auto it = nodes.begin();
    MarkedNode mn(node, mark, prev);
    while (it != nodes.end() && mark < it->mark) ++it;
    if (it == nodes.end()) nodes.push_back(mn);
    else nodes.insert(it, mn);
}

Way Dijkstra::unroll(map<Node*, MarkedNode>& visited, Node* begin, Node* curr) {
    Way way;
    way.length = visited[curr].mark;
    while (curr != begin) {
        way.nodes.push_back(curr);
        curr = visited[curr].prev;
    }
    way.nodes.push_back(begin);
    reverse(way.nodes.begin(), way.nodes.end());
    return way;
}

Way Dijkstra::shortestWay(Node* begin, Node* end) {
    PriorityQueue pq;
    pq.push(begin, 0, nullptr);
    map<Node*, MarkedNode> visited;

    while (!pq.empty()) {
        MarkedNode next = pq.pop();
        visited[next.node] = next;
        if (next.node == end) return unroll(visited, begin, end);

        for (node_iterator it = next.node->nb_begin(); it != next.node->nb_end(); ++it) {
            int weight = graph.getWeight(next.node, *it);
            if (weight == 0) weight = 1;
            int newMark = next.mark + weight;
            if (visited.find(*it) == visited.end()) {
                pq.push(*it, newMark, next.node);
            }
        }
    }
    return Way();
}
