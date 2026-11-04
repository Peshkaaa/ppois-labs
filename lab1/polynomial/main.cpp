/**
 * @file main.cpp
 * @author Peshkaaa
 * @brief Консольный калькулятор многочленов.
 * @details Весь ввод и вывод в консоль находится здесь, класс Polynomial
 * о консоли ничего не знает.
 */
#include <iostream>
#include <string>

#include "polynomial.h"

#ifdef _WIN32
#include <windows.h>
#endif

namespace {

const int kMenuCalculator = 1;
const int kMenuExit = 2;
const int kIgnoreLimit = 1000;

/// Читает многочлен A или B. При ошибке ввода очищает поток и возвращает false.
bool readPolynomial(const std::string& name, Polynomial& polynomial) {
    std::cout << "Многочлен " << name << " (степень и коэффициенты от свободного члена к старшему,\n"
              << "например \"2 5 -1 3\" это 3x^2 - x + 5): ";
    if (std::cin >> polynomial) {
        return true;
    }
    if (!std::cin.eof()) {
        std::cin.clear();
        std::cin.ignore(kIgnoreLimit, '\n');
        std::cout << "Неверный ввод многочлена\n";
    }
    return false;
}

/// Выводит результат деления, если делитель не нулевой.
void printDivision(const Polynomial& a, const Polynomial& b) {
    if (b.isZero()) {
        std::cout << "Деление на нулевой многочлен невозможно\n";
        return;
    }
    std::cout << "A / B = " << a / b << "\nA % B = " << a % b << '\n';
}

void calculator() {
    Polynomial a;
    Polynomial b;
    if (!readPolynomial("A", a) || !readPolynomial("B", b)) {
        return;
    }
    std::cout << "\nA = " << a << "\nB = " << b << "\nA + B = " << a + b << "\nA - B = " << a - b
              << "\nA * B = " << a * b << '\n';
    printDivision(a, b);
    double x = 0.0;
    std::cout << "Введите x для вычисления A(x) и B(x): ";
    if (std::cin >> x) {
        std::cout << "A(x) = " << a(x) << "\nB(x) = " << b(x) << '\n';
    }
}

}  // namespace

int main() {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif
    int choice = 0;
    while (choice != kMenuExit) {
        std::cout << "\n1. Калькулятор многочленов\n2. Выход\nВыбор: ";
        if (!(std::cin >> choice)) {
            if (std::cin.eof()) {
                break;
            }
            std::cin.clear();
            std::cin.ignore(kIgnoreLimit, '\n');
            choice = 0;
        }
        if (choice == kMenuCalculator) {
            calculator();
        } else if (choice != kMenuExit) {
            std::cout << "Выберите число от 1 до 2\n";
        }
    }
    return 0;
}
