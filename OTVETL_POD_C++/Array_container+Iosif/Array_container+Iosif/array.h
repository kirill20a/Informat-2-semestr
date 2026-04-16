#ifndef ARRAY_H
#define ARRAY_H

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

#endif // ARRAY_H


