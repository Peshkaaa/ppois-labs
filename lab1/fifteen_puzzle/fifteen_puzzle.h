/**
 * @file fifteen_puzzle.h
 * @author Peshkaaa
 * @brief Класс игры-головоломки «Пятнашки».
 * @details Класс хранит только состояние поля и правила игры, консоль
 * используется только в main.cpp.
 */
#ifndef FIFTEEN_PUZZLE_H
#define FIFTEEN_PUZZLE_H

#include <iosfwd>
#include <random>
#include <vector>

/// Направление, в котором перемещается пустая клетка.
enum class Direction { Up, Down, Left, Right };

/**
 * @brief Игра «Пятнашки» на квадратном поле.
 * @details Плитки хранятся числами 1..N*N-1, пустая клетка равна 0.
 * Конструктор копирования, оператор = и деструктор не объявлены вручную:
 * класс владеет только std::vector, поэтому копии, созданные компилятором,
 * получаются глубокими и корректными.
 */
class FifteenPuzzle {
public:
    /// Двумерная таблица плиток: tiles[строка][столбец].
    using Tiles = std::vector<std::vector<int>>;

    static constexpr int kDefaultSize = 4;  ///< Размер поля по умолчанию.
    static constexpr int kMinSize = 2;      ///< Минимальный размер поля.

    /**
     * @brief Создаёт собранную головоломку.
     * @param size Длина стороны поля.
     * @throw std::invalid_argument если size меньше kMinSize.
     */
    explicit FifteenPuzzle(int size = kDefaultSize);

    /**
     * @brief Создаёт головоломку с заданной расстановкой.
     * @param tiles Квадратная таблица с числами 0..N*N-1 без повторов.
     * @throw std::invalid_argument если таблица не квадратная, мала
     * или содержит неправильный набор чисел.
     */
    explicit FifteenPuzzle(const Tiles& tiles);

    /// @return Длина стороны поля.
    int size() const;

    /// @return Номер строки пустой клетки.
    int emptyRow() const;

    /// @return Номер столбца пустой клетки.
    int emptyColumn() const;

    /**
     * @brief Возвращает строку поля (значение клетки: puzzle[row][column]).
     * @param row Номер строки.
     * @return Значения клеток строки, пустая клетка равна 0.
     * @throw std::out_of_range если row вне поля.
     */
    const std::vector<int>& operator[](int row) const;

    /// @return true, если плитки стоят по порядку, а пустая клетка внизу справа.
    bool isSolved() const;

    /**
     * @brief Перемещает пустую клетку, соседняя плитка встаёт на её место.
     * @param direction Направление движения пустой клетки.
     * @return true, если ход выполнен; false, если мешает край поля.
     */
    bool move(Direction direction);

    /**
     * @brief Перемешивает поле случайными ходами из текущего состояния.
     * @param generator Генератор случайных чисел.
     * @details Так как используются только допустимые ходы, головоломку
     * всегда можно собрать. Результат никогда не равен собранному полю.
     */
    void shuffle(std::mt19937& generator);

    /// @return true, если расстановки плиток совпадают.
    bool operator==(const FifteenPuzzle& other) const;

    /// @return true, если расстановки плиток различаются.
    bool operator!=(const FifteenPuzzle& other) const;

private:
    Tiles tiles_;
    int emptyRow_;
    int emptyColumn_;
};

/**
 * @brief Записывает поле в поток: сначала размер, затем строки плиток.
 * @param output Поток вывода.
 * @param puzzle Головоломка.
 * @return output.
 */
std::ostream& operator<<(std::ostream& output, const FifteenPuzzle& puzzle);

/**
 * @brief Читает поле из потока в формате operator<<.
 * @param input Поток ввода.
 * @param puzzle Головоломка для результата.
 * @return input. При ошибке в потоке выставляется failbit, puzzle не меняется.
 */
std::istream& operator>>(std::istream& input, FifteenPuzzle& puzzle);

#endif  // FIFTEEN_PUZZLE_H
