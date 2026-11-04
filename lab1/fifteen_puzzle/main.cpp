/**
 * @file main.cpp
 * @author Peshkaaa
 * @brief Консольное меню и игровой цикл «Пятнашек».
 * @details Весь ввод и вывод в консоль находится здесь, класс FifteenPuzzle
 * о консоли ничего не знает.
 */
#include <iomanip>
#include <iostream>
#include <random>
#include <string>

#include "fifteen_puzzle.h"

#ifdef _WIN32
#include <windows.h>
#endif

namespace {

const int kMenuRules = 1;
const int kMenuPlay = 2;
const int kMenuExit = 3;
const int kCellWidth = 3;
const int kIgnoreLimit = 1000;
const std::string kDirectionKeys = "wsad";  // порядок как у enum Direction

/// Выводит поле: пустая клетка остаётся пробелом.
void printBoard(const FifteenPuzzle& puzzle) {
    std::cout << '\n';
    for (int row = 0; row < puzzle.size(); ++row) {
        for (int column = 0; column < puzzle.size(); ++column) {
            const int value = puzzle[row][column];
            if (value == 0) {
                std::cout << std::setw(kCellWidth) << ' ';
            } else {
                std::cout << std::setw(kCellWidth) << value;
            }
        }
        std::cout << '\n';
    }
}

/// Выполняет ход по клавише w/a/s/d. Возвращает false, если ход невозможен.
bool makeMove(FifteenPuzzle& puzzle, const std::string& command) {
    if (command.size() != 1) {
        return false;
    }
    const std::size_t index = kDirectionKeys.find(command[0]);
    return index != std::string::npos && puzzle.move(static_cast<Direction>(index));
}

void showRules() {
    std::cout << "\nПравила: расставьте числа по порядку от 1 до 15,\n"
              << "пустая клетка должна оказаться в правом нижнем углу.\n"
              << "w/a/s/d - переместить пустую клетку вверх/влево/вниз/вправо.\n";
}

void play(std::mt19937& generator) {
    FifteenPuzzle puzzle;
    puzzle.shuffle(generator);
    std::string command;
    while (true) {
        printBoard(puzzle);
        std::cout << "Ход (w/a/s/d), n - заново, q - выход: ";
        if (!(std::cin >> command) || command == "q") {
            return;
        }
        if (command == "n") {
            puzzle.shuffle(generator);
        } else if (!makeMove(puzzle, command)) {
            std::cout << "Неверная команда или невозможный ход\n";
        } else if (puzzle.isSolved()) {
            printBoard(puzzle);
            std::cout << "Победа!\n";
            return;
        }
    }
}

}  // namespace

int main() {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif
    std::random_device device;
    std::mt19937 generator(device());
    int choice = 0;
    while (choice != kMenuExit) {
        std::cout << "\n1. Правила\n2. Играть\n3. Выход\nВыбор: ";
        if (!(std::cin >> choice)) {
            if (std::cin.eof()) {
                break;
            }
            std::cin.clear();
            std::cin.ignore(kIgnoreLimit, '\n');
            choice = 0;
        }
        if (choice == kMenuRules) {
            showRules();
        } else if (choice == kMenuPlay) {
            play(generator);
        } else if (choice != kMenuExit) {
            std::cout << "Выберите число от 1 до 3\n";
        }
    }
    return 0;
}
