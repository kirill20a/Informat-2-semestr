#include "LineList.h"

// ========== –≈јЋ»«ј÷»я ARRAY ==========
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

// ========== –≈јЋ»«ј÷»я LINELIST ==========
LineList::LineList() : arr(10) {}

LineList::~LineList() {}

void LineList::push_back(int value) {
    arr.add(value);
}

void LineList::insert(int index, int value) {
    // ¬ставка в середину через массив (со сдвигом)
    if (index < 0 || index > arr.getSize()) return;

    // ƒобавл€ем в конец временно
    arr.add(value);

    // —двигаем элементы вправо до нужной позиции
    for (int i = arr.getSize() - 1; i > index; i--) {
        arr[i] = arr[i - 1];
    }
    arr[index] = value;
}

void LineList::removeAt(int index) {
    arr.removeAt(index);
}

int& LineList::operator[](int index) {
    return arr[index];
}

int LineList::getSize() const {
    return arr.getSize();
}

bool LineList::empty() const {
    return arr.getSize() == 0;
}

void LineList::clear() {
    while (arr.getSize() > 0) {
        arr.removeAt(0);
    }
}