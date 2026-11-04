/**
 * @file polynomial.h
 * @author Peshkaaa
 * @brief Класс многочлена от одной переменной.
 * @details Класс не работает с консолью: весь ввод и вывод в консоль
 * находится в main.cpp.
 */
#ifndef POLYNOMIAL_H
#define POLYNOMIAL_H

#include <iosfwd>
#include <vector>

/**
 * @brief Многочлен c0 + c1*x + c2*x^2 + ... + cn*x^n с вещественными коэффициентами.
 * @details Коэффициент при x^i хранится в элементе с номером i. Старшие
 * нулевые коэффициенты отбрасываются, поэтому степень многочлена всегда
 * настоящая, а нулевой многочлен имеет степень 0 и единственный коэффициент 0.
 * Конструктор копирования, оператор = и деструктор не объявлены вручную:
 * класс владеет только std::vector, поэтому копии, созданные компилятором,
 * получаются глубокими и корректными.
 * Сравнение коэффициентов точное (без допуска), как у обычных double.
 */
class Polynomial {
public:
    /// Создаёт нулевой многочлен.
    Polynomial();

    /**
     * @brief Создаёт многочлен по степени и массиву коэффициентов.
     * @param degree Степень многочлена (не меньше 0).
     * @param coefficients Коэффициенты от свободного члена к старшему,
     * ровно degree + 1 штук.
     * @throw std::invalid_argument если degree < 0 или размер массива
     * не равен degree + 1.
     */
    Polynomial(int degree, const std::vector<double>& coefficients);

    /// @return Степень многочлена (после отбрасывания старших нулей).
    int degree() const;

    /// @return true, если это нулевой многочлен.
    bool isZero() const;

    /**
     * @brief Возвращает коэффициент при x^power.
     * @param power Степень переменной.
     * @return Коэффициент.
     * @throw std::out_of_range если power < 0 или power > degree().
     */
    double operator[](int power) const;

    /**
     * @brief Вычисляет значение многочлена (схема Горнера).
     * @param x Значение переменной.
     * @return Значение многочлена в точке x.
     */
    double operator()(double x) const;

    /// @brief Прибавляет многочлен. @return Ссылка на себя.
    Polynomial& operator+=(const Polynomial& other);

    /// @brief Вычитает многочлен. @return Ссылка на себя.
    Polynomial& operator-=(const Polynomial& other);

    /// @brief Умножает на многочлен. @return Ссылка на себя.
    Polynomial& operator*=(const Polynomial& other);

    /**
     * @brief Заменяет многочлен частным от деления на other.
     * @return Ссылка на себя.
     * @throw std::domain_error если other нулевой.
     */
    Polynomial& operator/=(const Polynomial& other);

    /**
     * @brief Заменяет многочлен остатком от деления на other.
     * @return Ссылка на себя.
     * @throw std::domain_error если other нулевой.
     */
    Polynomial& operator%=(const Polynomial& other);

    /// @return Сумма многочленов.
    Polynomial operator+(const Polynomial& other) const;
    /// @return Разность многочленов.
    Polynomial operator-(const Polynomial& other) const;
    /// @return Произведение многочленов.
    Polynomial operator*(const Polynomial& other) const;
    /// @return Частное от деления в столбик. @throw std::domain_error при делении на ноль.
    Polynomial operator/(const Polynomial& other) const;
    /// @return Остаток от деления в столбик. @throw std::domain_error при делении на ноль.
    Polynomial operator%(const Polynomial& other) const;

    /// @return true, если все коэффициенты совпадают.
    bool operator==(const Polynomial& other) const;
    /// @return true, если какие-то коэффициенты различаются.
    bool operator!=(const Polynomial& other) const;

private:
    void trim();
    void divide(const Polynomial& divisor, Polynomial& quotient, Polynomial& remainder) const;

    std::vector<double> coefficients_;
};

/**
 * @brief Выводит многочлен в читаемом виде, например "3x^2 - x + 5".
 * @param output Поток вывода.
 * @param polynomial Многочлен.
 * @return output.
 */
std::ostream& operator<<(std::ostream& output, const Polynomial& polynomial);

/**
 * @brief Читает многочлен: степень, затем degree + 1 коэффициентов
 * от свободного члена к старшему (пример: "2 5 -1 3" это 3x^2 - x + 5).
 * @param input Поток ввода.
 * @param polynomial Многочлен для результата.
 * @return input. При ошибке в потоке выставляется failbit, polynomial не меняется.
 */
std::istream& operator>>(std::istream& input, Polynomial& polynomial);

#endif  // POLYNOMIAL_H
