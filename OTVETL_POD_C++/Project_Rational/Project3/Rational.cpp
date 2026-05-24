#include "Rational.h"
#include <cstdlib>
#include <cmath>
using namespace std;

// ========== ÂÑÏÎÌÎÃÀÒÅËÜÍÛÅ ÔÓÍÊÖÈÈ ==========

int Rational::gcd(int a, int b) const {
    a = abs(a);
    b = abs(b);
    while (b != 0) {
        int temp = b;
        b = a % b;
        a = temp;
    }
    return a;
}

void Rational::simplify() {
    if (denom == 0) {
        denom = 1;
    }

    int nod = gcd(numer, denom);
    if (nod > 1) {
        numer /= nod;
        denom /= nod;
    }

    if (denom < 0) {
        numer = -numer;
        denom = -denom;
    }
}

// ========== ÊÎÍÑÒĞÓÊÒÎĞÛ ==========

Rational::Rational() : numer(0), denom(1) {}

Rational::Rational(int number) : numer(number), denom(1) {}

Rational::Rational(int n, int d) : numer(n), denom(d) {
    if (denom == 0) {
        denom = 1;
    }
    simplify();
}

Rational::Rational(double x) {
    const double EPS = 1e-9;

    // Ïğîâåğêà íà öåëîå ÷èñëî
    double intPart;
    double fracPart = modf(x, &intPart);

    if (fabs(fracPart) < EPS) {
        numer = static_cast<int>(x);
        denom = 1;
    }
    else {
        // Íàõîäèì çíàìåíàòåëü
        int precision = 0;
        double temp = fracPart;
        while (fabs(temp - round(temp)) > EPS && precision < 9) {
            temp *= 10;
            precision++;
        }

        denom = static_cast<int>(pow(10, precision));
        numer = static_cast<int>(round(x * denom));
        simplify();
    }
}

// ========== ÊÎÍÑÒĞÓÊÒÎĞ ÊÎÏÈĞÎÂÀÍÈß ==========
Rational::Rational(const Rational& other) {
    numer = other.numer;
    denom = other.denom;
}

// ========== ÎÏÅĞÀÒÎĞ ÏĞÈÑÂÀÈÂÀÍÈß ==========
Rational& Rational::operator=(const Rational& other) {
    if (this != &other) {
        numer = other.numer;
        denom = other.denom;
    }
    return *this;
}

// ========== ÀĞÈÔÌÅÒÈ×ÅÑÊÈÅ ÎÏÅĞÀÒÎĞÛ ==========

Rational Rational::operator+(const Rational& r) const {
    Rational result;
    result.numer = numer * r.denom + denom * r.numer;
    result.denom = denom * r.denom;
    result.simplify();
    return result;
}

Rational Rational::operator-(const Rational& r) const {
    Rational result;
    result.numer = numer * r.denom - denom * r.numer;
    result.denom = denom * r.denom;
    result.simplify();
    return result;
}

Rational Rational::operator*(const Rational& r) const {
    Rational result;
    result.numer = numer * r.numer;
    result.denom = denom * r.denom;
    result.simplify();
    return result;
}

Rational Rational::operator/(const Rational& r) const {
    if (r.numer == 0) {
        return Rational(0, 1);
    }
    Rational result;
    result.numer = numer * r.denom;
    result.denom = denom * r.numer;
    result.simplify();
    return result;
}

// ========== ÑÎÑÒÀÂÍÛÅ ÎÏÅĞÀÒÎĞÛ ==========

Rational& Rational::operator+=(const Rational& r) {
    *this = *this + r;
    return *this;
}

Rational& Rational::operator-=(const Rational& r) {
    *this = *this - r;
    return *this;
}

Rational& Rational::operator*=(const Rational& r) {
    *this = *this * r;
    return *this;
}

Rational& Rational::operator/=(const Rational& r) {
    *this = *this / r;
    return *this;
}

// ========== ÓÍÀĞÍÛÅ ÎÏÅĞÀÒÎĞÛ ==========

Rational Rational::operator-() const {
    return Rational(-numer, denom);
}

Rational& Rational::operator++() {
    numer += denom;
    simplify();
    return *this;
}

Rational Rational::operator++(int) {
    Rational old = *this;
    numer += denom;
    simplify();
    return old;
}

Rational& Rational::operator--() {
    numer -= denom;
    simplify();
    return *this;
}

Rational Rational::operator--(int) {
    Rational old = *this;
    numer -= denom;
    simplify();
    return old;
}

// ========== ÎÏÅĞÀÒÎĞÛ ÑĞÀÂÍÅÍÈß ==========

bool Rational::operator==(const Rational& r) const {
    return (numer == r.numer) && (denom == r.denom);
}

bool Rational::operator!=(const Rational& r) const {
    return !(*this == r);
}

bool Rational::operator<(const Rational& r) const {
    return numer * r.denom < denom * r.numer;
}

bool Rational::operator>(const Rational& r) const {
    return numer * r.denom > denom * r.numer;
}

bool Rational::operator<=(const Rational& r) const {
    return !(*this > r);
}

bool Rational::operator>=(const Rational& r) const {
    return !(*this < r);
}

// ========== ÎÏÅĞÀÒÎĞÛ ÏĞÅÎÁĞÀÇÎÂÀÍÈß ==========

Rational::operator int() const {
    return numer / denom;
}

Rational::operator double() const {
    return static_cast<double>(numer) / denom;
}

// ========== ÎÏÅĞÀÒÎĞÛ ÂÂÎÄÀ/ÂÛÂÎÄÀ ==========

istream& operator>>(istream& in, Rational& r) {
    in >> r.numer >> r.denom;
    if (r.denom == 0) {
        r.denom = 1;
    }
    r.simplify();
    return in;
}

ostream& operator<<(ostream& out, const Rational& r) {
    if (r.denom == 1) {
        out << r.numer;
    }
    else {
        out << r.numer << "/" << r.denom;
    }
    return out;
}

// ========== ĞÅØÅÍÈÅ ÊÂÀÄĞÀÒÍÎÃÎ ÓĞÀÂÍÅÍÈß ==========
int Rational::solveQuadratic(int a, int b, int c, Rational& root1, Rational& root2) {
    // Îáğàáîòêà ëèíåéíîãî ñëó÷àÿ (a == 0)
    if (a == 0) {
        if (b == 0) {
            return 0;  // íåò êîğíåé (èëè áåñêîíå÷íî ìíîãî)
        }
        root1 = Rational(-c, b);
        return 1;  // îäèí êîğåíü
    }

    // Âû÷èñëÿåì äèñêğèìèíàíò
    int discriminant = b * b - 4 * a * c;

    // Íåò äåéñòâèòåëüíûõ êîğíåé
    if (discriminant < 0) {
        return 0;
    }

    // Îäèí êîğåíü (äèñêğèìèíàíò == 0)
    if (discriminant == 0) {
        root1 = Rational(-b, 2 * a);
        return 1;
    }

    // Äâà êîğíÿ
    // Ïğîâåğÿåì, ÿâëÿåòñÿ ëè äèñêğèìèíàíò ïîëíûì êâàäğàòîì
    int sqrt_discr = static_cast<int>(std::sqrt(discriminant));
    if (sqrt_discr * sqrt_discr != discriminant) {
        return 0;  // êîğíè íå ğàöèîíàëüíûå (èğğàöèîíàëüíûå)
    }

    root1 = Rational(-b - sqrt_discr, 2 * a);
    root2 = Rational(-b + sqrt_discr, 2 * a);
    return 2;
}
