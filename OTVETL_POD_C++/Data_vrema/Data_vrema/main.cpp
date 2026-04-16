#include <iostream>
#include "DateTime.h"

using namespace std;

int main_old() {
    setlocale(LC_ALL, "Russian");

    cout << "=== Демонстрация работы класса DateTime ===" << endl << endl;

    // 1. Конструкторы
    cout << "1. СОЗДАНИЕ ОБЪЕКТОВ:" << endl;
    cout << "----------------------------------------" << endl;

    DateTime dt1;  // по умолчанию
    cout << "dt1 (по умолчанию): " << dt1 << endl;

    DateTime dt2(21, 12, 2021, 7, 54, 34);
    cout << "dt2 (конструктор с параметрами): " << dt2 << endl;

    DateTime dt3("2024-12-25T10:30:00");
    cout << "dt3 (из ISO строки): " << dt3 << endl;

    DateTime dt4(dt2);
    cout << "dt4 (копия dt2): " << dt4 << endl;

    cout << endl;

    // 2. Геттеры и сеттеры
    cout << "2. ГЕТТЕРЫ И СЕТТЕРЫ:" << endl;
    cout << "----------------------------------------" << endl;

    cout << "dt2: " << dt2 << endl;
    cout << "  День: " << dt2.getDay() << endl;
    cout << "  Месяц: " << dt2.getMonth() << endl;
    cout << "  Год: " << dt2.getYear() << endl;
    cout << "  Час: " << dt2.getHour() << endl;
    cout << "  Минута: " << dt2.getMinute() << endl;
    cout << "  Секунда: " << dt2.getSecond() << endl;

    dt2.setDay(25);
    dt2.setMonth(12);
    dt2.setHour(23);
    cout << "После изменения: " << dt2 << endl;
    cout << endl;

    // 3. Проверка корректности
    cout << "3. ПРОВЕРКА КОРРЕКТНОСТИ:" << endl;
    cout << "----------------------------------------" << endl;

    cout << "dt2 корректна? " << (dt2.isValid() ? "Да" : "Нет") << endl;

    try {
        DateTime invalid("2024-02-30T12:00:00");
    }
    catch (DateTimeException& e) {
        cout << "Ошибка при создании неверной даты: " << e.message << endl;
    }

    try {
        dt2.setDay(32);
    }
    catch (DateTimeException& e) {
        cout << "Ошибка при установке дня: " << e.message << endl;
    }
    cout << endl;

    // 4. Форматированный вывод
    cout << "4. ФОРМАТИРОВАННЫЙ ВЫВОД:" << endl;
    cout << "----------------------------------------" << endl;

    cout << "ISO формат: " << dt3.toISOString() << endl;
    cout << "dd.mm.yyyy: " << dt3.toString("dd.mm.yyyy") << endl;
    cout << "dd MMMM yyyy: " << dt3.toString("dd MMMM yyyy") << endl;
    cout << "hh:mi:ss: " << dt3.toString("hh:mi:ss") << endl;
    cout << "Полный формат: " << dt3.toString("dd.mm.yyyy hh:mi:ss") << endl;
    cout << endl;

    // 5. День недели
    cout << "5. ДЕНЬ НЕДЕЛИ:" << endl;
    cout << "----------------------------------------" << endl;

    DateTime testDate(25, 12, 2024);
    cout << testDate << " - " << testDate.getWeekdayName() << endl;

    DateTime newYear(1, 1, 2025);
    cout << newYear << " - " << newYear.getWeekdayName() << endl;

    DateTime cosmosDay(12, 4, 1961);
    cout << cosmosDay << " - " << cosmosDay.getWeekdayName() << " (день первого полета человека в космос)" << endl;
    cout << endl;

    // 6. Разница между датами
    cout << "6. РАЗНИЦА МЕЖДУ ДАТАМИ:" << endl;
    cout << "----------------------------------------" << endl;

    DateTime start(1, 1, 2024);
    DateTime end(31, 12, 2024);

    cout << "Начало: " << start << endl;
    cout << "Конец: " << end << endl;
    cout << "Разница в днях: " << start.diffDays(end) << endl;
    cout << "Разница в секундах: " << start.diffSeconds(end) << endl;

    DateTime birth(12, 4, 1961);
    DateTime today(30, 3, 2026);
    cout << "\n12 апреля 1961 → " << today << endl;
    cout << "Прошло дней: " << birth.diffDays(today) << endl;
    cout << endl;

    // 7. Операции с датами
    cout << "7. ОПЕРАЦИИ С ДАТАМИ:" << endl;
    cout << "----------------------------------------" << endl;

    DateTime operation(1, 1, 2024);
    cout << "Исходная: " << operation << endl;

    operation.addDays(365);
    cout << "+365 дней: " << operation << endl;

    operation.addMonths(1);
    cout << "+1 месяц: " << operation << endl;

    operation.addYears(1);
    cout << "+1 год: " << operation << endl;

    operation.addHours(24);
    cout << "+24 часа: " << operation << endl;

    operation.addSeconds(3600);
    cout << "+3600 секунд: " << operation << endl;
    cout << endl;

    // 8. Сравнение дат
    cout << "8. СРАВНЕНИЕ ДАТ:" << endl;
    cout << "----------------------------------------" << endl;

    DateTime a(1, 1, 2024);
    DateTime b(31, 12, 2024);

    cout << a << " < " << b << "? " << (a < b ? "Да" : "Нет") << endl;
    cout << a << " > " << b << "? " << (a > b ? "Да" : "Нет") << endl;
    cout << a << " == " << a << "? " << (a == a ? "Да" : "Нет") << endl;
    cout << endl;
    // ========== НОВЫЕ ОПЕРАТОРЫ ==========
    cout << "\n=== НОВЫЕ ОПЕРАТОРЫ ===" << endl;
    cout << "----------------------------------------" << endl;

    DateTime now(1, 1, 2024, 0, 0, 0);
    cout << "Исходная дата: " << now << endl;

    DateTime future = now + 86400;  // +1 день
    cout << "now + 86400 секунд: " << future << endl;

    DateTime past = now - 86400;    // -1 день
    cout << "now - 86400 секунд: " << past << endl;

    DateTime later(2, 1, 2024, 0, 0, 0);
    int diff = later - now;
    cout << "Разница в секундах: " << diff << endl;

    DateTime result = now + 3600 + 60 + 1;
    cout << "now + 3600 + 60 + 1: " << result << endl;

    // ========== ДЕНЬ ПРОГРАММИСТА ==========
    cout << "\n=== ДЕНЬ ПРОГРАММИСТА ===" << endl;
    cout << "----------------------------------------" << endl;

    cout << "Введите дату в формате yyyy-MM-ddThh:mm:ss: ";
    DateTime userInput;
    try {
        cin >> userInput;

        int year = userInput.getYear();
        DateTime programmersDay = DateTime::getProgrammersDay(year);

        cout << "\nГод: " << year << endl;
        cout << "День программиста: " << programmersDay.toString("dd MMMM yyyy")
            << " (" << programmersDay.getWeekdayName() << ")" << endl;

        // Сравниваем только дату (без времени)
        DateTime userDateOnly(userInput.getDay(), userInput.getMonth(), userInput.getYear());
        DateTime progDayOnly(programmersDay.getDay(), programmersDay.getMonth(), programmersDay.getYear());

        if (userDateOnly < progDayOnly) {
            cout << "День программиста ещё не наступил (будет "
                << programmersDay.toString("dd MMMM") << ")" << endl;
        }
        else if (userDateOnly > progDayOnly) {
            cout << "День программиста уже прошёл (был "
                << programmersDay.toString("dd MMMM") << ")" << endl;
        }
        else {
            cout << "СЕГОДНЯ ДЕНЬ ПРОГРАММИСТА! Поздравляем!" << endl;
        }

    }
    catch (DateTimeException& e) {
        cout << "Ошибка: " << e.message << endl;
    }



    // 9. Ввод с клавиатуры
    cout << "9. ВВОД ДАТЫ С КЛАВИАТУРЫ:" << endl;
    cout << "----------------------------------------" << endl;
    cout << "Введите дату в формате yyyy-MM-ddThh:mm:ss: ";

    DateTime userDate;
    try {
        cin >> userDate;
        cout << "Вы ввели: " << userDate.toString("dd.mm.yyyy hh:mi:ss") << endl;
        cout << "День недели: " << userDate.getWeekdayName() << endl;
    }
    catch (DateTimeException& e) {
        cout << "Ошибка: " << e.message << endl;
    }

    cout << endl << "=== Программа завершена ===" << endl;

    return 0;
}