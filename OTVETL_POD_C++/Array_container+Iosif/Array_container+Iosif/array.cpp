#include "Array.h"

Array::Array(int startCapacity) : size(0), capacity(startCapacity) {
    data = new int[capacity];
}

Array::~Array() {
    delete[] data;
}

void Array::add(int value) {
    if (size == capacity) {
        capacity *= 2;
        int* newData = new int[capacity];
        for (int i = 0; i < size; i++) {
            newData[i] = data[i];
        }
        delete[] data;
        data = newData;
    }
    data[size++] = value;
}

void Array::removeAt(int index) {
    if (index < 0 || index >= size) return;

    for (int i = index; i < size - 1; i++) {
        data[i] = data[i + 1];
    }
    size--;
}

int& Array::operator[](int index) {
    return data[index];
}

int Array::getSize() const {
    return size;
}