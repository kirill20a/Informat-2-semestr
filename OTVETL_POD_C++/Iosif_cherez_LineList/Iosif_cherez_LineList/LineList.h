#ifndef LINELIST_H
#define LINELIST_H

#include <iostream>
using namespace std;

// Класс Array (динамический массив)
class Array {
private:
    int* data;
    int size;
    int capacity;

public:
    Array(int startCapacity = 10);
    ~Array();

    void add(int value);
    void removeAt(int index);
    int& operator[](int index);
    int getSize() const;
};

// Класс LineList (линейный список на основе Array)
class LineList {
private:
    Array arr;

public:
    LineList();
    ~LineList();

    void push_back(int value);
    void insert(int index, int value);
    void removeAt(int index);
    int& operator[](int index);
    int getSize() const;
    bool empty() const;
    void clear();
};

#endif // LINELIST_H