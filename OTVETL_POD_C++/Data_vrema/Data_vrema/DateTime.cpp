#include "DateTime.h"
#include <sstream>
#include <iomanip>
#include <cmath>

// ========== КОНСТРУКТОРЫ ==========

DateTime::DateTime() : year(1970), month(1), day(1), hour(0), minute(0), second(0) {}

DateTime::DateTime(int d, int m, int y, int h, int min, int sec) {
    if (!isValidDate(d, m, y)) {
        throw DateTimeException("Неверная дата!");
    }
    if (!isValidTime(h, min, sec)) {
        throw DateTimeException("Неверное время!");
    }
    day = d;
    month = m;
    year = y;
    hour = h;
    minute = min;
    second = sec;
}

DateTime::DateTime(const string& isoString) {
    // Формат: yyyy-MM-ddThh:mm:ss
    int y, m, d, h, min, sec;
    char dash1, dash2, T, colon1, colon2;

    istringstream iss(isoString);
    iss >> y >> dash1 >> m >> dash2 >> d >> T >> h >> colon1 >> min >> colon2 >> sec;

    if (iss.fail() || dash1 != '-' || dash2 != '-' || T != 'T' || colon1 != ':' || colon2 != ':') {
        throw DateTimeException("Неверный формат! Ожидается: yyyy-MM-ddThh:mm:ss");
    }

    if (!isValidDate(d, m, y)) {
        throw DateTimeException("Неверная дата!");
    }
    if (!isValidTime(h, min, sec)) {
        throw DateTimeException("Неверное время!");
    }

    year = y;
    month = m;
    day = d;
    hour = h;
    minute = min;
    second = sec;
}
//NEW
DateTime::DateTime(const string& ymd, const string& hms) {
    // ymd = "20230101" → 2023-01-01
    year = stoi(ymd.substr(0, 4));
    month = stoi(ymd.substr(4, 2));
    day = stoi(ymd.substr(6, 2));

    // hms = "000000" → 00:00:00
    hour = stoi(hms.substr(0, 2));
    minute = stoi(hms.substr(2, 2));
    second = stoi(hms.substr(4, 2));

    if (!isValid()) {
        throw DateTimeException("Неверная дата/время из файла!");
    }
}

DateTime::DateTime(const DateTime& other) {
    year = other.year;
    month = other.month;
    day = other.day;
    hour = other.hour;
    minute = other.minute;
    second = other.second;
}

DateTime::~DateTime() {}

// ========== ОПЕРАТОР ПРИСВАИВАНИЯ ==========

DateTime& DateTime::operator=(const DateTime& other) {
    if (this != &other) {
        year = other.year;
        month = other.month;
        day = other.day;
        hour = other.hour;
        minute = other.minute;
        second = other.second;
    }
    return *this;
}

// ========== ВСПОМОГАТЕЛЬНЫЕ МЕТОДЫ ==========

bool DateTime::isLeapYear(int y) const {
    return (y % 4 == 0 && y % 100 != 0) || (y % 400 == 0);
}

int DateTime::daysInMonth(int m, int y) const {
    if (m == 2) {
        return isLeapYear(y) ? 29 : 28;
    }
    if (m == 4 || m == 6 || m == 9 || m == 11) {
        return 30;
    }
    return 31;
}

bool DateTime::isValidDate(int d, int m, int y) const {
    if (y < 1 || y > 9999) return false;
    if (m < 1 || m > 12) return false;
    if (d < 1 || d > daysInMonth(m, y)) return false;
    return true;
}

bool DateTime::isValidTime(int h, int min, int sec) const {
    return (h >= 0 && h < 24) && (min >= 0 && min < 60) && (sec >= 0 && sec < 60);
}

long long DateTime::toDaysSinceEpoch() const {
    // Алгоритм вычисления количества дней от 0000-01-01
    long long totalDays = 0;

    // Добавляем дни за все года
    for (int y = 1; y < year; y++) {
        totalDays += isLeapYear(y) ? 366 : 365;
    }

    // Добавляем дни за месяцы текущего года
    for (int m = 1; m < month; m++) {
        totalDays += daysInMonth(m, year);
    }

    // Добавляем дни текущего месяца
    totalDays += day - 1;

    return totalDays;
}

void DateTime::fromDaysSinceEpoch(long long days) {
    // Находим год
    int y = 1;
    while (days >= (isLeapYear(y) ? 366 : 365)) {
        days -= (isLeapYear(y) ? 366 : 365);
        y++;
    }
    year = y;

    // Находим месяц
    int m = 1;
    while (days >= daysInMonth(m, year)) {
        days -= daysInMonth(m, year);
        m++;
    }
    month = m;
    day = days + 1;
}

// ========== ГЕТТЕРЫ ==========

int DateTime::getDay() const { return day; }
int DateTime::getMonth() const { return month; }
int DateTime::getYear() const { return year; }
int DateTime::getHour() const { return hour; }
int DateTime::getMinute() const { return minute; }
int DateTime::getSecond() const { return second; }

// ========== СЕТТЕРЫ ==========

void DateTime::setDay(int d) {
    if (isValidDate(d, month, year)) {
        day = d;
    }
    else {
        throw DateTimeException("Неверный день!");
    }
}

void DateTime::setMonth(int m) {
    if (isValidDate(day, m, year)) {
        month = m;
    }
    else {
        throw DateTimeException("Неверный месяц!");
    }
}

void DateTime::setYear(int y) {
    if (isValidDate(day, month, y)) {
        year = y;
    }
    else {
        throw DateTimeException("Неверный год!");
    }
}

void DateTime::setHour(int h) {
    if (h >= 0 && h < 24) {
        hour = h;
    }
    else {
        throw DateTimeException("Неверный час!");
    }
}

void DateTime::setMinute(int min) {
    if (min >= 0 && min < 60) {
        minute = min;
    }
    else {
        throw DateTimeException("Неверная минута!");
    }
}

void DateTime::setSecond(int sec) {
    if (sec >= 0 && sec < 60) {
        second = sec;
    }
    else {
        throw DateTimeException("Неверная секунда!");
    }
}

// ========== МЕТОДЫ ДЛЯ РАБОТЫ С ДАТОЙ ==========

void DateTime::addDays(int days) {
    long long totalDays = toDaysSinceEpoch() + days;
    fromDaysSinceEpoch(totalDays);
}

void DateTime::addMonths(int months) {
    int newMonth = month + months;
    int newYear = year;

    while (newMonth > 12) {
        newMonth -= 12;
        newYear++;
    }
    while (newMonth < 1) {
        newMonth += 12;
        newYear--;
    }

    month = newMonth;
    year = newYear;

    // Корректируем день, если он выходит за пределы месяца
    if (day > daysInMonth(month, year)) {
        day = daysInMonth(month, year);
    }
}

void DateTime::addYears(int years) {
    year += years;
    if (day > daysInMonth(month, year)) {
        day = daysInMonth(month, year);
    }
}

void DateTime::addHours(int hours) {
    addSeconds(hours * 3600);
}

void DateTime::addMinutes(int minutes) {
    addSeconds(minutes * 60);
}

void DateTime::addSeconds(int seconds) {
    long long totalSeconds = toDaysSinceEpoch() * 86400LL + hour * 3600 + minute * 60 + second + seconds;

    long long days = totalSeconds / 86400;
    long long remainingSeconds = totalSeconds % 86400;
    if (remainingSeconds < 0) {
        remainingSeconds += 86400;
        days--;
    }

    fromDaysSinceEpoch(days);
    hour = remainingSeconds / 3600;
    minute = (remainingSeconds % 3600) / 60;
    second = remainingSeconds % 60;
}

// ========== РАЗНИЦА МЕЖДУ ДАТАМИ ==========
long long DateTime::diffDays(const DateTime& other) const {
    return llabs(toDaysSinceEpoch() - other.toDaysSinceEpoch());
}

long long DateTime::diffSeconds(const DateTime& other) const {
    long long thisSeconds = toDaysSinceEpoch() * 86400LL + hour * 3600 + minute * 60 + second;
    long long otherSeconds = other.toDaysSinceEpoch() * 86400LL + other.hour * 3600 + other.minute * 60 + other.second;
    return llabs(thisSeconds - otherSeconds);
}

// ========== ДЕНЬ НЕДЕЛИ ==========
int DateTime::getWeekday() const {
    // Алгоритм Зеллера
    // Возвращает: 0 = суббота, 1 = воскресенье, 2 = понедельник, 
    //             3 = вторник, 4 = среда, 5 = четверг, 6 = пятница

    int d = day;
    int m = month;
    int y = year;

    // В алгоритме Зеллера январь и февраль считаются 13 и 14 месяцем предыдущего года
    if (m < 3) {
        m += 12;
        y--;
    }

    int K = y % 100;  // год в столетии
    int J = y / 100;  // столетие

    // Формула Зеллера
    int h = (d + (13 * (m + 1)) / 5 + K + (K / 4) + (J / 4) + 5 * J) % 7;

    // h: 0 = суббота, 1 = воскресенье, 2 = понедельник, 
    //    3 = вторник, 4 = среда, 5 = четверг, 6 = пятница

    return h;
}

string DateTime::getWeekdayName() const {
    int wd = getWeekday();
    const char* names[] = { "Суббота", "Воскресенье", "Понедельник",
                           "Вторник", "Среда", "Четверг", "Пятница" };
    return names[wd];
}

// ========== ПРОВЕРКА КОРРЕКТНОСТИ ==========

bool DateTime::isValid() const {
    return isValidDate(day, month, year) && isValidTime(hour, minute, second);
}

// ========== ОПЕРАТОРЫ СРАВНЕНИЯ ==========

bool DateTime::operator==(const DateTime& other) const {
    return toDaysSinceEpoch() == other.toDaysSinceEpoch() &&
        hour == other.hour && minute == other.minute && second == other.second;
}

bool DateTime::operator!=(const DateTime& other) const {
    return !(*this == other);
}

bool DateTime::operator<(const DateTime& other) const {
    long long thisDays = toDaysSinceEpoch();
    long long otherDays = other.toDaysSinceEpoch();
    if (thisDays != otherDays) return thisDays < otherDays;
    if (hour != other.hour) return hour < other.hour;
    if (minute != other.minute) return minute < other.minute;
    return second < other.second;
}

bool DateTime::operator<=(const DateTime& other) const {
    return !(*this > other);
}

bool DateTime::operator>(const DateTime& other) const {
    return other < *this;
}

bool DateTime::operator>=(const DateTime& other) const {
    return !(*this < other);
}

// ========== ФОРМАТИРОВАННЫЙ ВЫВОД ==========

string DateTime::toString(const string& format) const {
    string result = format;
    size_t pos;

    // Год
    pos = result.find("yyyy");
    if (pos != string::npos) {
        stringstream ss;
        ss << setw(4) << setfill('0') << year;
        result.replace(pos, 4, ss.str());
    }

    pos = result.find("yy");
    if (pos != string::npos) {
        stringstream ss;
        ss << setw(2) << setfill('0') << (year % 100);
        result.replace(pos, 2, ss.str());
    }

    // Месяц
    pos = result.find("mm");
    if (pos != string::npos) {
        stringstream ss;
        ss << setw(2) << setfill('0') << month;
        result.replace(pos, 2, ss.str());
    }

    pos = result.find("MMMM");
    if (pos != string::npos) {
        const char* months[] = { "января", "февраля", "марта", "апреля", "мая", "июня",
                                "июля", "августа", "сентября", "октября", "ноября", "декабря" };
        result.replace(pos, 4, months[month - 1]);
    }

    // День
    pos = result.find("dd");
    if (pos != string::npos) {
        stringstream ss;
        ss << setw(2) << setfill('0') << day;
        result.replace(pos, 2, ss.str());
    }

    // Час
    pos = result.find("hh");
    if (pos != string::npos) {
        stringstream ss;
        ss << setw(2) << setfill('0') << hour;
        result.replace(pos, 2, ss.str());
    }

    // Минута
    pos = result.find("mi");
    if (pos != string::npos) {
        stringstream ss;
        ss << setw(2) << setfill('0') << minute;
        result.replace(pos, 2, ss.str());
    }

    // Секунда
    pos = result.find("ss");
    if (pos != string::npos) {
        stringstream ss;
        ss << setw(2) << setfill('0') << second;
        result.replace(pos, 2, ss.str());
    }

    return result;
}

string DateTime::toISOString() const {
    stringstream ss;
    ss << setw(4) << setfill('0') << year << "-"
        << setw(2) << setfill('0') << month << "-"
        << setw(2) << setfill('0') << day << "T"
        << setw(2) << setfill('0') << hour << ":"
        << setw(2) << setfill('0') << minute << ":"
        << setw(2) << setfill('0') << second;
    return ss.str();
}


// ========== ВВОД/ВЫВОД ==========

ostream& operator<<(ostream& out, const DateTime& dt) {
    out << dt.toISOString();
    return out;
}

istream& operator>>(istream& in, DateTime& dt) {
    string input;
    in >> input;
    dt = DateTime(input);
    return in;
}

// ========== РЕАЛИЗАЦИЯ МЕТОДОВ ДЛЯ ДНЯ ПРОГРАММИСТА ==========

DateTime DateTime::getProgrammersDay(int year) {
    // День программиста — 256-й день года
    // 13 сентября в обычный год, 12 сентября в високосный

    DateTime startOfYear(1, 1, year);
    DateTime result = startOfYear;
    result.addDays(255);  // 256-й день (так как 1 января — 1-й день)

    return result;
}

bool DateTime::isProgrammersDay() const {
    // Проверяем, является ли текущая дата днём программиста
    DateTime programmersDay = getProgrammersDay(year);

    return (day == programmersDay.getDay() &&
        month == programmersDay.getMonth() &&
        year == programmersDay.getYear());
}


// ========== ОПЕРАТОРЫ АРИФМЕТИКИ ==========

// Дата + секунды
DateTime DateTime::operator+(int seconds) const {
    DateTime result = *this;
    result.addSeconds(seconds);
    return result;
}

// Дата - секунды
DateTime DateTime::operator-(int seconds) const {
    DateTime result = *this;
    result.addSeconds(-seconds);
    return result;
}

// Разница между датами в секундах (this - other)
int DateTime::operator-(const DateTime& other) const {
    return (int)diffSeconds(other);
}
//NEW
// Дата + дни
DateTime DateTime::operator+(double days) const {
    DateTime result = *this;
    result.addDays((int)days);
    return result;
}

// Дата - дни
DateTime DateTime::operator-(double days) const {
    DateTime result = *this;
    result.addDays(-(int)days);
    return result;
}
