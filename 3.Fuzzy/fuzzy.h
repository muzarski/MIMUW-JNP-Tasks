#ifndef FUZZY_H
#define FUZZY_H

#include <iostream>
#include <tuple>
#include <cmath>
#include <set>

using namespace std;
using real_t = double;

namespace utils {
    constexpr real_t min_of_3(real_t a, real_t b, real_t c) {
        return min(min(a, b), c);
    }

    constexpr real_t max_of_3(real_t a, real_t b, real_t c) {
        return max(max(a, b), c);
    }

    constexpr real_t mid_of_3(real_t a, real_t b, real_t c) {
        return a <= b ? (b <= c ? b : (a <= c ? c : a)) : (a <= c ? a : (b <= c ? c : b));
    }

    constexpr real_t calc_z(real_t l, real_t m, real_t u) {
        return (u - l) + sqrt(1 + (u - m) * (u - m)) + sqrt(1 + (m - l) * (m - l));
    }

    constexpr real_t calc_x(real_t l, real_t m, real_t u) {
        return ((u - l) * m + sqrt(1 + (u - m) * (u - m)) * l + sqrt(1 + (m - l) * (m - l)) * u) / calc_z(l, m, u);
    }

    constexpr real_t calc_y(real_t l, real_t m, real_t u) {
        return (u - l) / calc_z(l, m, u);
    }
}

class TriFuzzyNum {
    public:
        constexpr TriFuzzyNum(real_t arg1, real_t arg2, real_t arg3);
        constexpr TriFuzzyNum(const TriFuzzyNum &other);
        constexpr TriFuzzyNum(TriFuzzyNum &&other);
        constexpr TriFuzzyNum& operator=(const TriFuzzyNum &other);
        constexpr TriFuzzyNum& operator=(TriFuzzyNum &&other);
        constexpr TriFuzzyNum& operator+=(const TriFuzzyNum &other);
        constexpr TriFuzzyNum operator+(const TriFuzzyNum &other) const;
        constexpr TriFuzzyNum& operator-=(const TriFuzzyNum &other);
        constexpr TriFuzzyNum operator-(const TriFuzzyNum &other) const;
        constexpr TriFuzzyNum& operator*=(const TriFuzzyNum &other);
        constexpr TriFuzzyNum operator*(const TriFuzzyNum &other) const;
        constexpr bool operator==(const TriFuzzyNum &other) const;
        constexpr bool operator!=(const TriFuzzyNum &other) const;
        constexpr partial_ordering operator<=>(const TriFuzzyNum &other) const;
        constexpr const real_t& lower_value() const;
        constexpr const real_t& modal_value() const;
        constexpr const real_t& upper_value() const;
    private:
        real_t l;
        real_t m;
        real_t u;
        tuple<real_t, real_t, real_t> rank;
        friend ostream& operator<<(ostream&, const TriFuzzyNum&);
};

constexpr TriFuzzyNum::TriFuzzyNum(real_t arg1, real_t arg2, real_t arg3) : l(utils::min_of_3(arg1, arg2, arg3)),
                                                                            m(utils::mid_of_3(arg1, arg2, arg3)),
                                                                            u(utils::max_of_3(arg1, arg2, arg3)) {
    real_t x_this = utils::calc_x(l, m, u);
    real_t y_this = utils::calc_y(l, m, u);

    rank = make_tuple(x_this - y_this / 2, 1 - y_this, m);
}
constexpr TriFuzzyNum::TriFuzzyNum(const TriFuzzyNum &other) = default;
constexpr TriFuzzyNum::TriFuzzyNum(TriFuzzyNum &&other) : l(other.l), m(other.m), u(other.u), rank(move(other.rank)) { };

constexpr const real_t& TriFuzzyNum::lower_value() const {return l; };
constexpr const real_t& TriFuzzyNum::modal_value() const { return m; };
constexpr const real_t& TriFuzzyNum::upper_value() const { return u; };

constexpr TriFuzzyNum& TriFuzzyNum::operator=(const TriFuzzyNum &other) = default;
constexpr TriFuzzyNum& TriFuzzyNum::operator=(TriFuzzyNum &&other) {
    l = other.l;
    m = other.m;
    u = other.u;
    rank = move(other.rank);
    return *this;
}

constexpr TriFuzzyNum& TriFuzzyNum::operator+=(const TriFuzzyNum &other) {
    *this = TriFuzzyNum{l + other.l, m + other.m, u + other.u};
    return *this;
}
constexpr TriFuzzyNum TriFuzzyNum::operator+(const TriFuzzyNum &other) const {
    return TriFuzzyNum(*this) += other;
}

constexpr TriFuzzyNum& TriFuzzyNum::operator-=(const TriFuzzyNum &other) {
    *this = TriFuzzyNum{l - other.u, m - other.m, u - other.l};
    return *this;
}
constexpr TriFuzzyNum TriFuzzyNum::operator-(const TriFuzzyNum &other) const {
    return TriFuzzyNum(*this) -= other;
}

constexpr TriFuzzyNum& TriFuzzyNum::operator*=(const TriFuzzyNum &other) {
    *this = TriFuzzyNum{l * other.l, m * other.m, u * other.u};
    return *this;
}
constexpr TriFuzzyNum TriFuzzyNum::operator*(const TriFuzzyNum &other) const {
    return TriFuzzyNum(*this) *= other;
}

constexpr bool TriFuzzyNum::operator==(const TriFuzzyNum &other) const {
    return l == other.l && m == other.m && u == other.u;
}
constexpr bool TriFuzzyNum::operator!=(const TriFuzzyNum &other) const {
    return !(*this == other);
}

constexpr partial_ordering TriFuzzyNum::operator<=>(const TriFuzzyNum &other) const {
    return rank <=> other.rank;
}

ostream& operator<<(ostream &os, const TriFuzzyNum &num) {
    os << "("
       << num.l
       << ", " << num.m
       << ", " << num.u
       << ")";
    return os;
}

consteval TriFuzzyNum crisp_number(real_t v) {
    return {v, v, v};
}

constinit const TriFuzzyNum crisp_zero = crisp_number(0);

/* ----- SET ----- */

class TriFuzzyNumSet {
    public:
        TriFuzzyNumSet();
        TriFuzzyNumSet(const TriFuzzyNumSet &other);
        TriFuzzyNumSet(TriFuzzyNumSet &&other);
        TriFuzzyNumSet(initializer_list<TriFuzzyNum> args);
        TriFuzzyNumSet& operator=(const TriFuzzyNumSet &other);
        TriFuzzyNumSet& operator=(TriFuzzyNumSet &&other);
        void insert(const TriFuzzyNum &num);
        void insert(TriFuzzyNum &&num);
        void remove(const TriFuzzyNum &num);
        TriFuzzyNum arithmetic_mean() const;
    private:
        multiset<TriFuzzyNum> s;
};

TriFuzzyNumSet::TriFuzzyNumSet() = default;
TriFuzzyNumSet::TriFuzzyNumSet(const TriFuzzyNumSet &other) = default;
TriFuzzyNumSet::TriFuzzyNumSet(TriFuzzyNumSet &&other) : s(move(other.s)) { }
TriFuzzyNumSet::TriFuzzyNumSet(initializer_list<TriFuzzyNum> args) : s(args) { };

TriFuzzyNumSet& TriFuzzyNumSet::operator=(const TriFuzzyNumSet &other) = default;
TriFuzzyNumSet& TriFuzzyNumSet::operator=(TriFuzzyNumSet &&other) {
    s = move(other.s);
    return *this;
}

void TriFuzzyNumSet::insert(const TriFuzzyNum &num) { s.insert(num); }
void TriFuzzyNumSet::insert(TriFuzzyNum &&num) { s.insert(move(num)); }
void TriFuzzyNumSet::remove(const TriFuzzyNum &num) {
    // Deleting just one copy of the number
    auto iter = s.find(num);
    if (iter != s.end()) s.erase(iter);
}

TriFuzzyNum TriFuzzyNumSet::arithmetic_mean() const {
    if (s.empty()) throw length_error("TriFuzzyNumSet::arithmetic_mean - the set is empty.");

    real_t res_l = 0;
    real_t res_m = 0;
    real_t res_u = 0;

    for (const auto & iter : s) {
        res_l += iter.lower_value();
        res_m += iter.modal_value();
        res_u += iter.upper_value();
    }

    auto size = static_cast<real_t>(s.size());
    res_l /= size;
    res_m /= size;
    res_u /= size;
    return {res_l, res_m, res_u};
}

#endif // FUZZY_H
