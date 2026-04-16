#ifndef DATETIME_H
#define DATETIME_H

#include <iostream>
#include <string>
using namespace std;

// Класс исключения для ошибок даты
class DateTimeException {
public:
    string message;
    DateTimeException(const string& msg) : message(msg) {}
};

class DateTime {
private:
    int year;
    int month;
    int day;
    int hour;
    int minute;
    int second;

    // Вспомогательные методы
    bool isLeapYear(int y) const;
    int daysInMonth(int m, int y) const;
    bool isValidDate(int d, int m, int y) const;
    bool isValidTime(int h, int min, int sec) const;

    // Преобразование в количество дней от 0000-01-01
    long long toDaysSinceEpoch() const;
    void fromDaysSinceEpoch(long long days);

public:

    // Конструкторы
    DateTime();
    DateTime(int d, int m, int y, int h = 0, int min = 0, int sec = 0);
    DateTime(const string& ymd, const string& hms);//NEW
    DateTime(const string& isoString);  
    DateTime(const DateTime& other);

    // Деструктор
    ~DateTime();

    // Операторы присваивания
    DateTime& operator=(const DateTime& other);

    // Геттеры
    int getDay() const;
    int getMonth() const;
    int getYear() const;
    int getHour() const;
    int getMinute() const;
    int getSecond() const;

    // Сеттеры (с проверкой)
    void setDay(int d);
    void setMonth(int m);
    void setYear(int y);
    void setHour(int h);
    void setMinute(int min);
    void setSecond(int sec);

    // Методы для работы с датой
    void addDays(int days);
    void addMonths(int months);
    void addYears(int years);
    void addHours(int hours);
    void addMinutes(int minutes);
    void addSeconds(int seconds);

    // Разница между датами
    long long diffDays(const DateTime& other) const;
    long long diffSeconds(const DateTime& other) const;
    
    // Операторы арифметики
    DateTime operator+(int seconds) const;     // секунды
    DateTime operator-(int seconds) const;     // секунды
    DateTime operator+(double days) const;     // ← НОВЫЙ: дни
    DateTime operator-(double days) const;     // ← НОВЫЙ: дни
    int operator-(const DateTime& other) const;

    // День недели (0=воскресенье, 1=понедельник, ..., 6=суббота)
    int getWeekday() const;
    string getWeekdayName() const;

    // Проверка корректности
    bool isValid() const;

    // Сравнение
    bool operator==(const DateTime& other) const;
    bool operator!=(const DateTime& other) const;
    bool operator<(const DateTime& other) const;
    bool operator<=(const DateTime& other) const;
    bool operator>(const DateTime& other) const;
    bool operator>=(const DateTime& other) const;

    // Форматированный вывод
    string toString(const string& format = "dd.mm.yyyy hh:mi:ss") const;
    string toISOString() const;  // yyyy-MM-ddThh:mm:ss

    // Ввод/вывод
    friend ostream& operator<<(ostream& out, const DateTime& dt);
    friend istream& operator>>(istream& in, DateTime& dt);

    // ========== МЕТОДЫ ДЛЯ ДНЯ ПРОГРАММИСТА ==========

    // Возвращает дату дня программиста для заданного года
    static DateTime getProgrammersDay(int year);

    // Проверяет, является ли дата днём программиста
    bool isProgrammersDay() const;
};

#endif // DATETIME_H