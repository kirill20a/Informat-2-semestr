#include "Graph.h"
#include <algorithm>

using namespace std;

// ========== РЕАЛИЗАЦИЯ МЕТОДОВ PATH ==========

// Конструктор копирования
Path::Path(const Path& other) : len(other.len) {
    for (int i = 0; i < len; i++) {
        nodes[i] = other.nodes[i];
    }
}

// Оператор присваивания
Path& Path::operator=(const Path& other) {
    if (this != &other) {
        len = other.len;
        for (int i = 0; i < len; i++) {
            nodes[i] = other.nodes[i];
        }
    }
    return *this;
}

// Доступ по индексу
Node* Path::operator[](int i) const {
    return (i >= 0 && i < len) ? nodes[i] : nullptr;
}

// Приведение к bool
Path::operator bool() const {
    return len > 0;
}

// Добавить в конец
void Path::pushBack(Node* node) {
    if (len < 1000) nodes[len++] = node;
}

// Удалить последний
void Path::popBack() {
    if (len > 0) len--;
}

// Получить по индексу
Node* Path::get(int i) const {
    return (i >= 0 && i < len) ? nodes[i] : nullptr;
}

// Длина пути
int Path::getLength() const {
    return len;
}

// Очистка
void Path::clear() {
    len = 0;
}

// Проверка на пустоту
bool Path::empty() const {
    return len == 0;
}

// Вывод в поток
void Path::print(ostream& out) const {
    for (int i = 0; i < len; i++) {
        out << nodes[i]->getName();
        if (i < len - 1) out << " -> ";
    }
}

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

// сравнение и присваивание — только по имени
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

void Graph::addNode(Node* node) {
    if (node) nodes.insert(node);
}

// удаление вершины: 4 шага (из множества, ссылки у соседей, рёбра, память)
void Graph::removeNode(Node* node) {
    if (!node) return;
    nodes.erase(node);// убрали из множества
    for (Node* other : nodes)// удаляем ссылки у соседей
        other->removeNeighbour(node);
    for (auto it = edges.begin(); it != edges.end();) {
        if (it->from == node || it->to == node)
            it = edges.erase(it);
        else
            ++it;
    }
    delete node;// освободили память
}

void Graph::addEdge(Node* begin, Node* end, int weight) {
    if (!begin || !end) return;
    if (!hasNode(begin) || !hasNode(end)) return;
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
    for (const Edge& e : edges)
        if ((e.from == begin && e.to == end) || (e.from == end && e.to == begin))
            return e.weight;
    return 0;
}

node_iterator Graph::begin() const { return nodes.begin(); }
node_iterator Graph::end() const { return nodes.end(); }
size_t Graph::getNodeCount() const { return nodes.size(); }

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
ostream& operator<<(ostream& out, const Path& path) {
    if (path.getLength() == 0) {
        out << "путь не найден";
        return out;
    }
    for (int i = 0; i < path.getLength(); i++) {
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
            Node* nb = *it;
            if (nb == end) return true;
            if (!visited.count(nb)) {
                visited.insert(nb);
                q.push(nb);
            }
        }
    }
    return false;
}

// BFS-путь (упрощённо — без восстановления)
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
        for (int i = 0; i < sz; i++) { // проверяем соседей текущего уровня->
            Node* cur = q.front(); q.pop();
            for (auto it = cur->nb_begin(); it != cur->nb_end(); ++it) {
                Node* nb = *it;
                if (nb == end) return dist;
                if (!visited.count(nb)) {
                    visited.insert(nb);
                    q.push(nb);
                }
            }
        }
    }
    return -1;
}

// ---------- DFS ----------зашли — пометили — 
// пошли в соседа — если не нашли, возвращаемся.
DFS::DFS(const Graph& agraph) : graph(agraph) {}

bool DFS::connectedRecursive(Node* begin, Node* end, int) {
    if (begin == end) return true;
    visited.insert(begin);
    for (auto it = begin->nb_begin(); it != begin->nb_end(); ++it) {
        Node* nb = *it;
        if (!visited.count(nb) && connectedRecursive(nb, end, 0))
            return true;
    }
    return false;
}

bool DFS::connected(Node* begin, Node* end) {
    visited.clear();
    return connectedRecursive(begin, end, 0);
}

// рекурсивное восстановление пути(Строит путь.
// Если не нашли — откатываем последнюю вершину.)
bool DFS::findPathRecursive(Node* cur, Node* end, Path& path, set<Node*>& seen) {
    path.pushBack(cur);
    if (cur == end) return true;
    seen.insert(cur);
    for (auto it = cur->nb_begin(); it != cur->nb_end(); ++it) {
        Node* nb = *it;
        if (!seen.count(nb) && findPathRecursive(nb, end, path, seen))
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
// ========== РЕАЛИЗАЦИЯ ОПЕРАТОРОВ СРАВНЕНИЯ ДЛЯ EDGE ==========
//Нужны для хранения в set (сортировка).
bool Graph::Edge::operator<(const Edge& other) const {
    if (from != other.from) return from < other.from;
    return to < other.to;
}
//Учитывает, что граф неориентированный: (a,b) == (b,a).
bool Graph::Edge::operator==(const Edge& other) const {
    return (from == other.from && to == other.to) ||
        (from == other.to && to == other.from);
}

bool Graph::Edge::operator!=(const Edge& other) const {
    return !(*this == other);
}