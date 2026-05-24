#ifndef RATIONAL_H
#define RATIONAL_H

#include <iostream>
using namespace std;

class Rational {
private:
    int numer;
    int denom;

    void simplify();
    int gcd(int a, int b) const;

public:

    // Конструкторы
    Rational();
    Rational(int number);
    Rational(int n, int d);
    Rational(double x);

    // Конструктор копирования
    Rational(const Rational& other);

    // Оператор присваивания
    Rational& operator=(const Rational& other);

    // Арифметические операторы
    Rational operator+(const Rational& r) const;
    Rational operator-(const Rational& r) const;
    Rational operator*(const Rational& r) const;
    Rational operator/(const Rational& r) const;

    // Составные операторы
    Rational& operator+=(const Rational& r);
    Rational& operator-=(const Rational& r);
    Rational& operator*=(const Rational& r);
    Rational& operator/=(const Rational& r);

    // Унарные операторы
    Rational operator-() const;
    Rational& operator++();    // префиксный
    Rational operator++(int);  // постфиксный
    Rational& operator--();    // префиксный
    Rational operator--(int);  // постфиксный

    // Операторы сравнения
    bool operator==(const Rational& r) const;
    bool operator!=(const Rational& r) const;
    bool operator<(const Rational& r) const;
    bool operator>(const Rational& r) const;
    bool operator<=(const Rational& r) const;
    bool operator>=(const Rational& r) const;

    // Операторы преобразования
    operator int() const;
    operator double() const;

    // Геттеры
    int getNumer() const { return numer; }
    int getDenom() const { return denom; }

    // Ввод/вывод
    friend istream& operator>>(istream& in, Rational& r);
    friend ostream& operator<<(ostream& out, const Rational& r);
    static int solveQuadratic(int a, int b, int c, Rational& root1, Rational& root2);

};

#endif // RATIONAL_H