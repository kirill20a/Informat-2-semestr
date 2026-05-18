#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include "DateTime.h"

using namespace std;

// Структура для хранения записи
struct MoonRecord {
    DateTime time;
    double el;  // угол места
};

// Определение формата файла по году
bool isOldFormat(int year) {
    return year <= 2008;
}

// Чтение файла в старом формате (2000-2008)
MoonRecord* readOldFormatFile(const string& filename, const DateTime& targetDate, int& recordCount) {
    ifstream file(filename);
    if (!file.is_open()) {
        throw DateTimeException("Не удалось открыть файл: " + filename);
    }

    MoonRecord* dayRecords = new MoonRecord[200];
    int index = 0;

    string line;
    getline(file, line); // пропускаем заголовок

    int targetYear = targetDate.getYear();
    int targetMonth = targetDate.getMonth();
    int targetDay = targetDate.getDay();

    while (getline(file, line)) {
        if (line.empty()) continue;

        // Убираем кавычки, если есть
        if (line.front() == '"') {
            line = line.substr(1, line.length() - 2);
        }

        string ymd, hms;
        double r, h, az, sh, dl;

        istringstream iss(line);
        iss >> ymd >> hms >> r >> h >> az >> sh >> dl;

        int year = stoi(ymd.substr(0, 4));
        int month = stoi(ymd.substr(4, 2));
        int day = stoi(ymd.substr(6, 2));

        if (year > targetYear || (year == targetYear && month > targetMonth) ||
            (year == targetYear && month == targetMonth && day > targetDay)) {
            break;
        }

        if (year == targetYear && month == targetMonth && day == targetDay) {
            DateTime dt(ymd, hms);
            dayRecords[index].time = dt;
            dayRecords[index].el = h;
            index++;
        }
    }

    file.close();
    recordCount = index;
    return dayRecords;
}

// Чтение файла в новом формате (2009+)
MoonRecord* readNewFormatFile(const string& filename, const DateTime& targetDate, int& recordCount) {
    ifstream file(filename);
    if (!file.is_open()) {
        throw DateTimeException("Не удалось открыть файл: " + filename);
    }

    MoonRecord* dayRecords = new MoonRecord[90000];
    int index = 0;

    string line;
    getline(file, line); // пропускаем заголовок

    int targetYear = targetDate.getYear();
    int targetMonth = targetDate.getMonth();
    int targetDay = targetDate.getDay();

    while (getline(file, line)) {
        if (line.empty()) continue;

        string ymd, hms;
        double t, r, el, az, fi, lg;

        istringstream iss(line);
        iss >> ymd >> hms >> t >> r >> el >> az >> fi >> lg;

        int year = stoi(ymd.substr(0, 4));
        int month = stoi(ymd.substr(4, 2));
        int day = stoi(ymd.substr(6, 2));

        if (year > targetYear || (year == targetYear && month > targetMonth) ||
            (year == targetYear && month == targetMonth && day > targetDay)) {
            break;
        }

        if (year == targetYear && month == targetMonth && day == targetDay) {
            DateTime dt(ymd, hms);
            dayRecords[index].time = dt;
            dayRecords[index].el = el;
            index++;
        }
    }

    file.close();
    recordCount = index;
    return dayRecords;
}

// Универсальная функция чтения
MoonRecord* readMoonFileForDate(const string& filename, const DateTime& targetDate, int& recordCount, int year) {
    if (isOldFormat(year)) {
        return readOldFormatFile(filename, targetDate, recordCount);
    }
    else {
        return readNewFormatFile(filename, targetDate, recordCount);
    }
}

// Поиск событий (восход, кульминация, заход)
void findMoonEvents(MoonRecord* records, int recordCount, const DateTime& targetDate) {
    if (recordCount == 0 || records == nullptr) {
        cout << "Нет данных за указанную дату!" << endl;
        return;
    }

    DateTime riseTime, setTime, culminationTime;
    double maxEl = -1000;
    bool foundRise = false;
    bool foundSet = false;

    for (int i = 1; i < recordCount; i++) {
        double el1 = records[i - 1].el;
        double el2 = records[i].el;

        if (!foundRise && el1 < 0 && el2 >= 0) {
            riseTime = records[i].time;
            foundRise = true;
        }

        if (!foundSet && el1 >= 0 && el2 < 0) {
            setTime = records[i].time;
            foundSet = true;
        }

        if (el2 > maxEl) {
            maxEl = el2;
            culminationTime = records[i].time;
        }
    }

    cout << "\nДата: " << setw(2) << setfill('0') << targetDate.getDay() << "."
        << setw(2) << setfill('0') << targetDate.getMonth() << "."
        << targetDate.getYear() << endl;

    if (foundRise) {
        cout << "Восход Луны: " << riseTime.toString("hh:mi:ss") << endl;
    }
    else {
        cout << "Восход Луны: не найден" << endl;
    }

    cout << "Кульминация Луны: " << culminationTime.toString("hh:mi:ss") << endl;

    if (foundSet) {
        cout << "Заход Луны: " << setTime.toString("hh:mi:ss") << endl;
    }
    else {
        cout << "Заход Луны: не найден" << endl;
    }
}

int main() {
    setlocale(LC_ALL, "Russian");
    cout << "   ЗАДАЧА ПРО ЛУНУ" << endl;
    cout << "========================================" << endl << endl;

    cout << "Введите дату в формате дд.мм.гггг: ";
    int d, m, y;
    char dot1, dot2;
    cin >> d >> dot1 >> m >> dot2 >> y;

    if (dot1 != '.' || dot2 != '.') {
        cout << "Ошибка: неверный формат даты!" << endl;
        return 1;
    }

    DateTime targetDate(d, m, y);
    string filename = "moon" + to_string(y) + ".dat";

    cout << "\nФайл: " << filename << endl;
    cout << "Дата: " << d << "." << m << "." << y << endl;
    cout << "----------------------------------------" << endl;

    try {
        int recordCount = 0;
        MoonRecord* records = readMoonFileForDate(filename, targetDate, recordCount, y);

        if (records == nullptr || recordCount == 0) {
            cout << "Нет данных за указанную дату!" << endl;
            return 1;
        }

        cout << "Записей за день: " << recordCount << endl;

        findMoonEvents(records, recordCount, targetDate);

        delete[] records;

    }
    catch (const DateTimeException& e) {
        cout << "Ошибка: " << e.message << endl;
    }

    return 0;
}