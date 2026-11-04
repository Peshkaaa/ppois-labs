/**
 * @file fifteen_puzzle.cpp
 * @author Peshkaaa
 * @brief Реализация класса FifteenPuzzle.
 */
#include "fifteen_puzzle.h"

#include <istream>
#include <ostream>
#include <stdexcept>

namespace {

const int kDirectionCount = 4;
const int kShuffleMoves = 1000;

void checkSize(int size) {
    if (size < FifteenPuzzle::kMinSize) {
        throw std::invalid_argument("Размер поля должен быть не меньше 2");
    }
}

}  // namespace

FifteenPuzzle::FifteenPuzzle(int size) : emptyRow_(size - 1), emptyColumn_(size - 1) {
    checkSize(size);
    const int total = size * size;
    tiles_.assign(size, std::vector<int>(size));
    for (int index = 0; index < total; ++index) {
        tiles_[index / size][index % size] = (index + 1) % total;
    }
}

FifteenPuzzle::FifteenPuzzle(const Tiles& tiles) : tiles_(tiles), emptyRow_(0), emptyColumn_(0) {
    const int size = static_cast<int>(tiles.size());
    checkSize(size);
    std::vector<bool> used(size * size, false);
    for (int row = 0; row < size; ++row) {
        if (static_cast<int>(tiles[row].size()) != size) {
            throw std::invalid_argument("Поле должно быть квадратным");
        }
        for (int column = 0; column < size; ++column) {
            const int value = tiles[row][column];
            if (value < 0 || value >= size * size || used[value]) {
                throw std::invalid_argument("Плитки должны быть числами 0..N*N-1 без повторов");
            }
            used[value] = true;
            if (value == 0) {
                emptyRow_ = row;
                emptyColumn_ = column;
            }
        }
    }
}

int FifteenPuzzle::size() const {
    return static_cast<int>(tiles_.size());
}

int FifteenPuzzle::emptyRow() const {
    return emptyRow_;
}

int FifteenPuzzle::emptyColumn() const {
    return emptyColumn_;
}

const std::vector<int>& FifteenPuzzle::operator[](int row) const {
    return tiles_.at(row);
}

bool FifteenPuzzle::isSolved() const {
    const int total = size() * size();
    for (int index = 0; index < total; ++index) {
        if (tiles_[index / size()][index % size()] != (index + 1) % total) {
            return false;
        }
    }
    return true;
}

bool FifteenPuzzle::move(Direction direction) {
    int row = emptyRow_;
    int column = emptyColumn_;
    switch (direction) {
        case Direction::Up: --row; break;
        case Direction::Down: ++row; break;
        case Direction::Left: --column; break;
        case Direction::Right: ++column; break;
    }
    if (row < 0 || row >= size() || column < 0 || column >= size()) {
        return false;
    }
    std::swap(tiles_[row][column], tiles_[emptyRow_][emptyColumn_]);
    emptyRow_ = row;
    emptyColumn_ = column;
    return true;
}

void FifteenPuzzle::shuffle(std::mt19937& generator) {
    std::uniform_int_distribution<int> pick(0, kDirectionCount - 1);
    do {
        for (int step = 0; step < kShuffleMoves; ++step) {
            move(static_cast<Direction>(pick(generator)));
        }
    } while (isSolved());
}

bool FifteenPuzzle::operator==(const FifteenPuzzle& other) const {
    return tiles_ == other.tiles_;
}

bool FifteenPuzzle::operator!=(const FifteenPuzzle& other) const {
    return !(*this == other);
}

std::ostream& operator<<(std::ostream& output, const FifteenPuzzle& puzzle) {
    output << puzzle.size() << '\n';
    for (int row = 0; row < puzzle.size(); ++row) {
        for (int column = 0; column < puzzle.size(); ++column) {
            output << (column > 0 ? " " : "") << puzzle[row][column];
        }
        output << '\n';
    }
    return output;
}

std::istream& operator>>(std::istream& input, FifteenPuzzle& puzzle) {
    int size = 0;
    if (!(input >> size)) {
        return input;
    }
    FifteenPuzzle::Tiles tiles;
    for (int row = 0; row < size && input; ++row) {
        std::vector<int> cells;
        int value = 0;
        while (static_cast<int>(cells.size()) < size && input >> value) {
            cells.push_back(value);
        }
        tiles.push_back(cells);
    }
    if (!input) {
        return input;
    }
    try {
        puzzle = FifteenPuzzle(tiles);
    } catch (const std::invalid_argument&) {
        input.setstate(std::ios::failbit);
    }
    return input;
}
