/**
 * @file tests.cpp
 * @brief Модульные тесты класса FifteenPuzzle (UnitTest++).
 */
#include <UnitTest++/UnitTest++.h>

#include <algorithm>
#include <random>
#include <sstream>
#include <stdexcept>

#include "fifteen_puzzle.h"

using Tiles = FifteenPuzzle::Tiles;

namespace {

const unsigned kSeed = 12345;

/// Поле 4x4 с пустой клеткой в центре (строка 1, столбец 1).
Tiles emptyInCenter() {
    return {{1, 2, 3, 4}, {5, 0, 6, 8}, {9, 10, 7, 11}, {13, 14, 15, 12}};
}

}  // namespace

TEST(DefaultSizeIsFour) {
    FifteenPuzzle puzzle;
    CHECK_EQUAL(4, puzzle.size());
}

TEST(CustomSizeIsStored) {
    FifteenPuzzle puzzle(3);
    CHECK_EQUAL(3, puzzle.size());
}

TEST(SizeBelowMinimumThrows) {
    CHECK_THROW(FifteenPuzzle(1), std::invalid_argument);
    CHECK_THROW(FifteenPuzzle(-5), std::invalid_argument);
}

TEST(NewPuzzleIsSolved) {
    FifteenPuzzle puzzle;
    CHECK(puzzle.isSolved());
    CHECK_EQUAL(1, puzzle[0][0]);
    CHECK_EQUAL(15, puzzle[3][2]);
    CHECK_EQUAL(0, puzzle[3][3]);
}

TEST(NewPuzzleEmptyCellIsBottomRight) {
    FifteenPuzzle puzzle(3);
    CHECK_EQUAL(2, puzzle.emptyRow());
    CHECK_EQUAL(2, puzzle.emptyColumn());
}

TEST(TilesConstructorStoresValues) {
    FifteenPuzzle puzzle(emptyInCenter());
    CHECK_EQUAL(6, puzzle[1][2]);
    CHECK_EQUAL(12, puzzle[3][3]);
    CHECK(!puzzle.isSolved());
}

TEST(TilesConstructorFindsEmptyCell) {
    FifteenPuzzle puzzle(emptyInCenter());
    CHECK_EQUAL(1, puzzle.emptyRow());
    CHECK_EQUAL(1, puzzle.emptyColumn());
}

TEST(TooSmallTilesThrow) {
    Tiles tiles = {{0}};
    CHECK_THROW(FifteenPuzzle puzzle(tiles), std::invalid_argument);
}

TEST(NonSquareTilesThrow) {
    Tiles tiles = {{1, 2, 3}, {4, 5, 0}};
    CHECK_THROW(FifteenPuzzle puzzle(tiles), std::invalid_argument);
}

TEST(DuplicateTilesThrow) {
    Tiles tiles = {{1, 1}, {2, 0}};
    CHECK_THROW(FifteenPuzzle puzzle(tiles), std::invalid_argument);
}

TEST(TooBigTileThrows) {
    Tiles tiles = {{1, 2}, {7, 0}};
    CHECK_THROW(FifteenPuzzle puzzle(tiles), std::invalid_argument);
}

TEST(NegativeTileThrows) {
    Tiles tiles = {{1, 2}, {-1, 0}};
    CHECK_THROW(FifteenPuzzle puzzle(tiles), std::invalid_argument);
}

TEST(BracketsReturnRow) {
    FifteenPuzzle puzzle(emptyInCenter());
    CHECK_EQUAL(4, static_cast<int>(puzzle[2].size()));
    CHECK_EQUAL(9, puzzle[2][0]);
}

TEST(BracketsRowOutOfRangeThrows) {
    FifteenPuzzle puzzle;
    CHECK_THROW(puzzle[4], std::out_of_range);
}

TEST(MoveUpSwapsWithTileAbove) {
    FifteenPuzzle puzzle;
    CHECK(puzzle.move(Direction::Up));
    CHECK_EQUAL(0, puzzle[2][3]);
    CHECK_EQUAL(12, puzzle[3][3]);
    CHECK_EQUAL(2, puzzle.emptyRow());
}

TEST(MoveLeftSwapsWithTileOnLeft) {
    FifteenPuzzle puzzle;
    CHECK(puzzle.move(Direction::Left));
    CHECK_EQUAL(0, puzzle[3][2]);
    CHECK_EQUAL(15, puzzle[3][3]);
    CHECK_EQUAL(2, puzzle.emptyColumn());
}

TEST(MoveDownSwapsWithTileBelow) {
    FifteenPuzzle puzzle(emptyInCenter());
    CHECK(puzzle.move(Direction::Down));
    CHECK_EQUAL(0, puzzle[2][1]);
    CHECK_EQUAL(10, puzzle[1][1]);
}

TEST(MoveRightSwapsWithTileOnRight) {
    FifteenPuzzle puzzle(emptyInCenter());
    CHECK(puzzle.move(Direction::Right));
    CHECK_EQUAL(0, puzzle[1][2]);
    CHECK_EQUAL(6, puzzle[1][1]);
}

TEST(MoveDownAndRightFailAtBottomRightCorner) {
    FifteenPuzzle puzzle;
    CHECK(!puzzle.move(Direction::Down));
    CHECK(!puzzle.move(Direction::Right));
    CHECK(puzzle.isSolved());
}

TEST(MoveUpAndLeftFailAtTopLeftCorner) {
    Tiles tiles = {{0, 1}, {3, 2}};
    FifteenPuzzle puzzle(tiles);
    CHECK(!puzzle.move(Direction::Up));
    CHECK(!puzzle.move(Direction::Left));
    CHECK_EQUAL(0, puzzle[0][0]);
}

TEST(MoveAndBackRestoresState) {
    FifteenPuzzle puzzle(emptyInCenter());
    const FifteenPuzzle before = puzzle;
    CHECK(puzzle.move(Direction::Up));
    CHECK(puzzle.move(Direction::Down));
    CHECK(puzzle == before);
}

TEST(IsSolvedFalseAfterMove) {
    FifteenPuzzle puzzle;
    puzzle.move(Direction::Up);
    CHECK(!puzzle.isSolved());
    puzzle.move(Direction::Down);
    CHECK(puzzle.isSolved());
}

TEST(ShuffleKeepsAllNumbers) {
    std::mt19937 generator(kSeed);
    FifteenPuzzle puzzle;
    puzzle.shuffle(generator);
    std::vector<int> values;
    for (int row = 0; row < puzzle.size(); ++row) {
        values.insert(values.end(), puzzle[row].begin(), puzzle[row].end());
    }
    std::sort(values.begin(), values.end());
    for (int number = 0; number < 16; ++number) {
        CHECK_EQUAL(number, values[number]);
    }
}

TEST(ShuffleResultIsNotSolved) {
    std::mt19937 generator(kSeed);
    FifteenPuzzle puzzle;
    puzzle.shuffle(generator);
    CHECK(!puzzle.isSolved());
}

TEST(ShuffleWithSameSeedGivesSameBoard) {
    std::mt19937 first(kSeed);
    std::mt19937 second(kSeed);
    FifteenPuzzle left;
    FifteenPuzzle right;
    left.shuffle(first);
    right.shuffle(second);
    CHECK(left == right);
}

TEST(ShuffleUpdatesEmptyCellPosition) {
    std::mt19937 generator(kSeed);
    FifteenPuzzle puzzle;
    puzzle.shuffle(generator);
    CHECK_EQUAL(0, puzzle[puzzle.emptyRow()][puzzle.emptyColumn()]);
}

TEST(ShuffleWorksOnSmallBoard) {
    std::mt19937 generator(kSeed);
    FifteenPuzzle puzzle(2);
    puzzle.shuffle(generator);
    CHECK(!puzzle.isSolved());
}

TEST(CopyConstructorMakesIndependentCopy) {
    FifteenPuzzle original;
    FifteenPuzzle copy(original);
    CHECK(copy == original);
    copy.move(Direction::Up);
    CHECK(copy != original);
    CHECK(original.isSolved());
}

TEST(AssignmentCopiesState) {
    FifteenPuzzle source(emptyInCenter());
    FifteenPuzzle target(3);
    target = source;
    CHECK(target == source);
    CHECK_EQUAL(4, target.size());
    CHECK_EQUAL(1, target.emptyRow());
}

TEST(SelfAssignmentKeepsState) {
    FifteenPuzzle puzzle(emptyInCenter());
    FifteenPuzzle& alias = puzzle;
    puzzle = alias;
    CHECK_EQUAL(6, puzzle[1][2]);
}

TEST(EqualPuzzlesAreEqual) {
    FifteenPuzzle first;
    FifteenPuzzle second;
    CHECK(first == second);
    CHECK(!(first != second));
}

TEST(DifferentBoardsAreNotEqual) {
    FifteenPuzzle first;
    FifteenPuzzle second;
    second.move(Direction::Left);
    CHECK(first != second);
    CHECK(!(first == second));
}

TEST(DifferentSizesAreNotEqual) {
    FifteenPuzzle small(3);
    FifteenPuzzle big(4);
    CHECK(small != big);
}

TEST(OutputWritesSizeAndRows) {
    std::ostringstream output;
    output << FifteenPuzzle(2);
    CHECK_EQUAL(std::string("2\n1 2\n3 0\n"), output.str());
}

TEST(InputReadsWhatOutputWrote) {
    FifteenPuzzle original(emptyInCenter());
    std::stringstream stream;
    stream << original;
    FifteenPuzzle loaded;
    stream >> loaded;
    CHECK(!stream.fail());
    CHECK(loaded == original);
    CHECK_EQUAL(1, loaded.emptyRow());
}

TEST(InputWithDuplicateTilesFailsAndKeepsPuzzle) {
    std::istringstream input("2\n1 1\n2 0\n");
    FifteenPuzzle puzzle;
    input >> puzzle;
    CHECK(input.fail());
    CHECK(puzzle.isSolved());
    CHECK_EQUAL(4, puzzle.size());
}

TEST(InputTruncatedFails) {
    std::istringstream input("3\n1 2 3\n4 5\n");
    FifteenPuzzle puzzle;
    input >> puzzle;
    CHECK(input.fail());
}

TEST(InputWithoutSizeFails) {
    std::istringstream input("abc");
    FifteenPuzzle puzzle;
    input >> puzzle;
    CHECK(input.fail());
}

TEST(InputWithTooSmallSizeFails) {
    std::istringstream input("1\n0\n");
    FifteenPuzzle puzzle;
    input >> puzzle;
    CHECK(input.fail());
}

TEST(SolvedTwoByTwoHasExpectedValues) {
    FifteenPuzzle puzzle(2);
    CHECK_EQUAL(1, puzzle[0][0]);
    CHECK_EQUAL(2, puzzle[0][1]);
    CHECK_EQUAL(3, puzzle[1][0]);
    CHECK_EQUAL(0, puzzle[1][1]);
}

TEST(SolvedFiveByFiveHasLastTile24) {
    FifteenPuzzle puzzle(5);
    CHECK_EQUAL(24, puzzle[4][3]);
    CHECK_EQUAL(0, puzzle[4][4]);
    CHECK(puzzle.isSolved());
}

TEST(MoveLeftAcrossWholeRowThenFails) {
    FifteenPuzzle puzzle(3);
    CHECK(puzzle.move(Direction::Left));
    CHECK(puzzle.move(Direction::Left));
    CHECK(!puzzle.move(Direction::Left));
    CHECK_EQUAL(0, puzzle.emptyColumn());
}

TEST(MoveUpAcrossWholeColumnThenFails) {
    FifteenPuzzle puzzle(3);
    CHECK(puzzle.move(Direction::Up));
    CHECK(puzzle.move(Direction::Up));
    CHECK(!puzzle.move(Direction::Up));
    CHECK_EQUAL(0, puzzle.emptyRow());
}

TEST(MoveChangesOnlyTwoCells) {
    FifteenPuzzle before;
    FifteenPuzzle after;
    after.move(Direction::Up);
    int different = 0;
    for (int row = 0; row < 4; ++row) {
        for (int column = 0; column < 4; ++column) {
            different += before[row][column] != after[row][column] ? 1 : 0;
        }
    }
    CHECK_EQUAL(2, different);
}

TEST(ManyMovesKeepAllNumbers) {
    FifteenPuzzle puzzle;
    const Direction cycle[] = {Direction::Up, Direction::Left, Direction::Down, Direction::Right};
    for (int step = 0; step < 100; ++step) {
        puzzle.move(cycle[step % 4]);
    }
    std::vector<int> values;
    for (int row = 0; row < puzzle.size(); ++row) {
        values.insert(values.end(), puzzle[row].begin(), puzzle[row].end());
    }
    std::sort(values.begin(), values.end());
    for (int number = 0; number < 16; ++number) {
        CHECK_EQUAL(number, values[number]);
    }
}

TEST(ShuffleThreeByThreeIsNotSolved) {
    std::mt19937 generator(kSeed);
    FifteenPuzzle puzzle(3);
    puzzle.shuffle(generator);
    CHECK(!puzzle.isSolved());
}

TEST(ShuffleFiveByFiveIsNotSolved) {
    std::mt19937 generator(kSeed);
    FifteenPuzzle puzzle(5);
    puzzle.shuffle(generator);
    CHECK(!puzzle.isSolved());
}

TEST(ShuffleWithDifferentSeedsGivesDifferentBoards) {
    std::mt19937 first(1);
    std::mt19937 second(2);
    FifteenPuzzle left;
    FifteenPuzzle right;
    left.shuffle(first);
    right.shuffle(second);
    CHECK(left != right);
}

TEST(ShuffleTwiceGivesDifferentBoards) {
    std::mt19937 generator(kSeed);
    FifteenPuzzle puzzle;
    puzzle.shuffle(generator);
    const FifteenPuzzle firstResult = puzzle;
    puzzle.shuffle(generator);
    CHECK(puzzle != firstResult);
}

TEST(ShuffledTilesAreAcceptedByConstructor) {
    std::mt19937 generator(kSeed);
    FifteenPuzzle puzzle;
    puzzle.shuffle(generator);
    Tiles tiles;
    for (int row = 0; row < puzzle.size(); ++row) {
        tiles.push_back(puzzle[row]);
    }
    FifteenPuzzle rebuilt(tiles);
    CHECK(rebuilt == puzzle);
    CHECK_EQUAL(puzzle.emptyRow(), rebuilt.emptyRow());
    CHECK_EQUAL(puzzle.emptyColumn(), rebuilt.emptyColumn());
}

TEST(SwappedTilesAreNotSolved) {
    Tiles tiles = {{2, 1}, {3, 0}};
    FifteenPuzzle puzzle(tiles);
    CHECK(!puzzle.isSolved());
}

TEST(SolvedTilesEqualDefaultPuzzle) {
    Tiles tiles = {{1, 2}, {3, 0}};
    CHECK(FifteenPuzzle(tiles) == FifteenPuzzle(2));
}

TEST(BracketsWorkOnConstPuzzle) {
    const FifteenPuzzle puzzle(3);
    CHECK_EQUAL(5, puzzle[1][1]);
}

TEST(NegativeRowThrows) {
    FifteenPuzzle puzzle;
    CHECK_THROW(puzzle[-1], std::out_of_range);
}

TEST(MoreRowsThanColumnsThrows) {
    Tiles tiles = {{1, 2}, {3, 0}, {4, 5}};
    CHECK_THROW(FifteenPuzzle puzzle(tiles), std::invalid_argument);
}

TEST(OutputAfterMove) {
    FifteenPuzzle puzzle(2);
    puzzle.move(Direction::Up);
    std::ostringstream output;
    output << puzzle;
    CHECK_EQUAL(std::string("2\n1 0\n3 2\n"), output.str());
}

TEST(InputToleratesExtraWhitespace) {
    std::istringstream input("  2 \n\n 1   2\n 3\t0 \n");
    FifteenPuzzle puzzle(3);
    input >> puzzle;
    CHECK(!input.fail());
    CHECK(puzzle == FifteenPuzzle(2));
}

TEST(InputTwoPuzzlesFromOneStream) {
    std::istringstream input("2\n1 2\n3 0\n2\n1 0\n3 2\n");
    FifteenPuzzle first;
    FifteenPuzzle second;
    input >> first >> second;
    CHECK(!input.fail());
    CHECK(first.isSolved());
    CHECK(!second.isSolved());
}

TEST(InputValueOutOfRangeFails) {
    std::istringstream input("2\n1 2\n3 9\n");
    FifteenPuzzle puzzle;
    input >> puzzle;
    CHECK(input.fail());
}

TEST(InputNegativeSizeFails) {
    std::istringstream input("-3\n1 2 3\n");
    FifteenPuzzle puzzle;
    input >> puzzle;
    CHECK(input.fail());
}

int main() {
    return UnitTest::RunAllTests();
}
