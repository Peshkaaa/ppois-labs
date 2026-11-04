/**
 * @file polynomial.cpp
 * @author Peshkaaa
 * @brief Реализация класса Polynomial.
 */
#include "polynomial.h"

#include <cmath>
#include <istream>
#include <ostream>
#include <stdexcept>

Polynomial::Polynomial() : coefficients_(1, 0.0) {}

Polynomial::Polynomial(int degree, const std::vector<double>& coefficients) : coefficients_(coefficients) {
    if (degree < 0 || static_cast<int>(coefficients.size()) != degree + 1) {
        throw std::invalid_argument("Нужно ровно degree + 1 коэффициентов, degree >= 0");
    }
    trim();
}

int Polynomial::degree() const {
    return static_cast<int>(coefficients_.size()) - 1;
}

bool Polynomial::isZero() const {
    return degree() == 0 && coefficients_[0] == 0.0;
}

double Polynomial::operator[](int power) const {
    if (power < 0 || power > degree()) {
        throw std::out_of_range("Нет коэффициента с такой степенью");
    }
    return coefficients_[power];
}

double Polynomial::operator()(double x) const {
    double result = 0.0;
    for (int power = degree(); power >= 0; --power) {
        result = result * x + coefficients_[power];
    }
    return result;
}

Polynomial& Polynomial::operator+=(const Polynomial& other) {
    if (other.coefficients_.size() > coefficients_.size()) {
        coefficients_.resize(other.coefficients_.size(), 0.0);
    }
    for (std::size_t i = 0; i < other.coefficients_.size(); ++i) {
        coefficients_[i] += other.coefficients_[i];
    }
    trim();
    return *this;
}

Polynomial& Polynomial::operator-=(const Polynomial& other) {
    if (other.coefficients_.size() > coefficients_.size()) {
        coefficients_.resize(other.coefficients_.size(), 0.0);
    }
    for (std::size_t i = 0; i < other.coefficients_.size(); ++i) {
        coefficients_[i] -= other.coefficients_[i];
    }
    trim();
    return *this;
}

Polynomial& Polynomial::operator*=(const Polynomial& other) {
    std::vector<double> product(coefficients_.size() + other.coefficients_.size() - 1, 0.0);
    for (std::size_t i = 0; i < coefficients_.size(); ++i) {
        for (std::size_t j = 0; j < other.coefficients_.size(); ++j) {
            product[i + j] += coefficients_[i] * other.coefficients_[j];
        }
    }
    coefficients_ = product;
    trim();
    return *this;
}

Polynomial& Polynomial::operator/=(const Polynomial& other) {
    Polynomial quotient;
    Polynomial remainder;
    divide(other, quotient, remainder);
    *this = quotient;
    return *this;
}

Polynomial& Polynomial::operator%=(const Polynomial& other) {
    Polynomial quotient;
    Polynomial remainder;
    divide(other, quotient, remainder);
    *this = remainder;
    return *this;
}

Polynomial Polynomial::operator+(const Polynomial& other) const {
    Polynomial result(*this);
    result += other;
    return result;
}

Polynomial Polynomial::operator-(const Polynomial& other) const {
    Polynomial result(*this);
    result -= other;
    return result;
}

Polynomial Polynomial::operator*(const Polynomial& other) const {
    Polynomial result(*this);
    result *= other;
    return result;
}

Polynomial Polynomial::operator/(const Polynomial& other) const {
    Polynomial result(*this);
    result /= other;
    return result;
}

Polynomial Polynomial::operator%(const Polynomial& other) const {
    Polynomial result(*this);
    result %= other;
    return result;
}

bool Polynomial::operator==(const Polynomial& other) const {
    return coefficients_ == other.coefficients_;
}

bool Polynomial::operator!=(const Polynomial& other) const {
    return !(*this == other);
}

/// Отбрасывает старшие нулевые коэффициенты (но оставляет хотя бы один).
void Polynomial::trim() {
    while (coefficients_.size() > 1 && coefficients_.back() == 0.0) {
        coefficients_.pop_back();
    }
}

/// Деление в столбик: *this = divisor * quotient + remainder.
void Polynomial::divide(const Polynomial& divisor, Polynomial& quotient, Polynomial& remainder) const {
    if (divisor.isZero()) {
        throw std::domain_error("Деление на нулевой многочлен");
    }
    const std::size_t quotientSize = degree() >= divisor.degree() ? degree() - divisor.degree() + 1 : 1;
    quotient.coefficients_.assign(quotientSize, 0.0);
    remainder = *this;
    while (!remainder.isZero() && remainder.degree() >= divisor.degree()) {
        const int shift = remainder.degree() - divisor.degree();
        const double factor = remainder.coefficients_.back() / divisor.coefficients_.back();
        quotient.coefficients_[shift] = factor;
        for (int i = 0; i <= divisor.degree(); ++i) {
            remainder.coefficients_[i + shift] -= factor * divisor.coefficients_[i];
        }
        remainder.coefficients_.back() = 0.0;  // старший член сокращается точно
        remainder.trim();
    }
    quotient.trim();
}

std::ostream& operator<<(std::ostream& output, const Polynomial& polynomial) {
    if (polynomial.isZero()) {
        return output << 0;
    }
    bool first = true;
    for (int power = polynomial.degree(); power >= 0; --power) {
        const double coefficient = polynomial[power];
        if (coefficient == 0.0) {
            continue;
        }
        if (first) {
            output << (coefficient < 0 ? "-" : "");
        } else {
            output << (coefficient < 0 ? " - " : " + ");
        }
        if (std::abs(coefficient) != 1.0 || power == 0) {
            output << std::abs(coefficient);
        }
        if (power >= 1) {
            output << "x";
        }
        if (power > 1) {
            output << "^" << power;
        }
        first = false;
    }
    return output;
}

std::istream& operator>>(std::istream& input, Polynomial& polynomial) {
    int degree = 0;
    if (!(input >> degree)) {
        return input;
    }
    if (degree < 0) {
        input.setstate(std::ios::failbit);
        return input;
    }
    std::vector<double> coefficients;
    double value = 0.0;
    while (static_cast<int>(coefficients.size()) < degree + 1 && input >> value) {
        coefficients.push_back(value);
    }
    if (input) {
        polynomial = Polynomial(degree, coefficients);
    }
    return input;
}
