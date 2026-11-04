/**
 * @file tests_polynomial.cpp
 * @brief Модульные тесты класса Polynomial (UnitTest++).
 */
#include <UnitTest++/UnitTest++.h>

#include <sstream>
#include <stdexcept>
#include <string>

#include "polynomial.h"

namespace {

/// Строит многочлен по коэффициентам от свободного члена к старшему.
Polynomial make(const std::vector<double>& coefficients) {
    return Polynomial(static_cast<int>(coefficients.size()) - 1, coefficients);
}

std::string toString(const Polynomial& polynomial) {
    std::ostringstream output;
    output << polynomial;
    return output.str();
}

}  // namespace

TEST(DefaultPolynomialIsZero) {
    Polynomial p;
    CHECK(p.isZero());
    CHECK_EQUAL(0, p.degree());
    CHECK_EQUAL(0.0, p[0]);
}

TEST(ConstructorStoresCoefficients) {
    Polynomial p(2, {1, 2, 3});
    CHECK_EQUAL(2, p.degree());
    CHECK_EQUAL(1.0, p[0]);
    CHECK_EQUAL(2.0, p[1]);
    CHECK_EQUAL(3.0, p[2]);
}

TEST(ConstructorTrimsLeadingZeros) {
    Polynomial p(3, {1, 2, 0, 0});
    CHECK_EQUAL(1, p.degree());
}

TEST(AllZeroCoefficientsGiveZeroPolynomial) {
    Polynomial p(2, {0, 0, 0});
    CHECK(p.isZero());
    CHECK_EQUAL(0, p.degree());
}

TEST(NonZeroConstantIsNotZero) {
    Polynomial p(0, {5});
    CHECK(!p.isZero());
}

TEST(NegativeDegreeThrows) {
    CHECK_THROW(Polynomial(-1, {}), std::invalid_argument);
}

TEST(WrongCoefficientCountThrows) {
    CHECK_THROW(Polynomial(1, {1, 2, 3}), std::invalid_argument);
    CHECK_THROW(Polynomial(3, {1, 2}), std::invalid_argument);
}

TEST(BracketsOutOfRangeThrow) {
    Polynomial p(1, {1, 2});
    CHECK_THROW(p[2], std::out_of_range);
    CHECK_THROW(p[-1], std::out_of_range);
}

TEST(EvaluateAtPoint) {
    Polynomial p(2, {1, 2, 3});
    CHECK_EQUAL(17.0, p(2.0));
}

TEST(EvaluateAtZeroGivesFreeTerm) {
    Polynomial p(2, {7, 2, 3});
    CHECK_EQUAL(7.0, p(0.0));
}

TEST(EvaluateNegativeArgument) {
    Polynomial p(2, {1, 2, 3});
    CHECK_EQUAL(2.0, p(-1.0));
}

TEST(EvaluateZeroPolynomial) {
    Polynomial p;
    CHECK_EQUAL(0.0, p(123.0));
}

TEST(AddSameDegree) {
    Polynomial sum = make({1, 2, 3}) + make({4, 5, 6});
    CHECK(sum == make({5, 7, 9}));
}

TEST(AddDifferentDegreesInBothOrders) {
    Polynomial first = make({1, 2});
    Polynomial second = make({1, 1, 1, 1});
    CHECK(first + second == make({2, 3, 1, 1}));
    CHECK(second + first == make({2, 3, 1, 1}));
}

TEST(AddCancelsLeadingTerm) {
    Polynomial sum = make({1, 0, 3}) + make({0, 0, -3});
    CHECK_EQUAL(0, sum.degree());
    CHECK(sum == make({1}));
}

TEST(PlusEqualsReturnsReferenceToSelf) {
    Polynomial a = make({1, 1});
    Polynomial b = make({1});
    Polynomial& result = (a += b);
    CHECK(&result == &a);
    CHECK(a == make({2, 1}));
}

TEST(PlusDoesNotChangeOperands) {
    Polynomial a = make({1, 1});
    Polynomial b = make({2, 2});
    Polynomial sum = a + b;
    CHECK(a == make({1, 1}));
    CHECK(b == make({2, 2}));
    CHECK(sum == make({3, 3}));
}

TEST(Subtract) {
    CHECK(make({5, 7, 9}) - make({4, 5, 6}) == make({1, 2, 3}));
}

TEST(SubtractSelfGivesZero) {
    Polynomial p = make({1, 2, 3});
    p -= p;
    CHECK(p.isZero());
}

TEST(MinusEqualsWithHigherDegree) {
    Polynomial a = make({1});
    a -= make({0, 0, 2});
    CHECK(a == make({1, 0, -2}));
}

TEST(MultiplyDifferenceOfSquares) {
    Polynomial product = make({1, 1}) * make({-1, 1});
    CHECK(product == make({-1, 0, 1}));
}

TEST(MultiplyByZeroGivesZero) {
    CHECK((make({1, 2, 3}) * Polynomial()).isZero());
}

TEST(MultiplyByConstant) {
    CHECK(make({1, 2, 3}) * make({2}) == make({2, 4, 6}));
}

TEST(MultiplyEqualsSelfSquares) {
    Polynomial p = make({1, 1});
    p *= p;
    CHECK(p == make({1, 2, 1}));
}

TEST(DivideExactly) {
    Polynomial quotient = make({-1, 0, 1}) / make({-1, 1});
    CHECK(quotient == make({1, 1}));
}

TEST(DivideWithRemainder) {
    Polynomial dividend = make({1, 0, 1});
    Polynomial divisor = make({-1, 1});
    CHECK(dividend / divisor == make({1, 1}));
    CHECK(dividend % divisor == make({2}));
}

TEST(DivideByLowerDegreeDividendGivesZeroQuotient) {
    Polynomial dividend = make({0, 1});
    Polynomial divisor = make({0, 0, 1});
    CHECK((dividend / divisor).isZero());
    CHECK(dividend % divisor == dividend);
}

TEST(DivideByConstant) {
    CHECK(make({4, 2}) / make({2}) == make({2, 1}));
    CHECK((make({4, 2}) % make({2})).isZero());
}

TEST(DivideByZeroThrows) {
    CHECK_THROW(make({1, 1}) / Polynomial(), std::domain_error);
    CHECK_THROW(make({1, 1}) % Polynomial(), std::domain_error);
}

TEST(DivideEqualsAndModuloEquals) {
    Polynomial quotient = make({1, 0, 1});
    Polynomial remainder = make({1, 0, 1});
    quotient /= make({-1, 1});
    remainder %= make({-1, 1});
    CHECK(quotient == make({1, 1}));
    CHECK(remainder == make({2}));
}

TEST(DivisionIdentityHolds) {
    Polynomial a = make({1, 2, 3, 4});
    Polynomial b = make({1, 2});
    CHECK(a == (a / b) * b + a % b);
}

TEST(EqualPolynomialsAreEqual) {
    CHECK(make({1, 2}) == make({1, 2}));
    CHECK(!(make({1, 2}) != make({1, 2})));
}

TEST(DifferentPolynomialsAreNotEqual) {
    CHECK(make({1, 2}) != make({1, 3}));
    CHECK(make({1, 2}) != make({1, 2, 3}));
}

TEST(CopyIsIndependent) {
    Polynomial original = make({1, 2});
    Polynomial copy(original);
    copy += make({1});
    CHECK(original == make({1, 2}));
    CHECK(copy == make({2, 2}));
}

TEST(AssignmentCopiesCoefficients) {
    Polynomial source = make({1, 2, 3});
    Polynomial target;
    target = source;
    CHECK(target == source);
}

TEST(SelfAssignmentKeepsValue) {
    Polynomial p = make({1, 2, 3});
    Polynomial& alias = p;
    p = alias;
    CHECK(p == make({1, 2, 3}));
}

TEST(OutputMixedSigns) {
    CHECK_EQUAL(std::string("3x^2 - x + 5"), toString(make({5, -1, 3})));
}

TEST(OutputSkipsZeroTerms) {
    CHECK_EQUAL(std::string("x^3 + 2"), toString(make({2, 0, 0, 1})));
}

TEST(OutputNegativeLeadingTerm) {
    CHECK_EQUAL(std::string("-x^2 - 2x"), toString(make({0, -2, -1})));
}

TEST(OutputZeroAndConstants) {
    CHECK_EQUAL(std::string("0"), toString(Polynomial()));
    CHECK_EQUAL(std::string("-4"), toString(make({-4})));
}

TEST(OutputFractionalCoefficient) {
    CHECK_EQUAL(std::string("2.5x"), toString(make({0, 2.5})));
}

TEST(InputReadsDegreeAndCoefficients) {
    std::istringstream input("2 5 -1 3");
    Polynomial p;
    input >> p;
    CHECK(!input.fail());
    CHECK(p == make({5, -1, 3}));
}

TEST(InputNegativeDegreeFails) {
    std::istringstream input("-2 1 2 3");
    Polynomial p = make({9});
    input >> p;
    CHECK(input.fail());
    CHECK(p == make({9}));
}

TEST(InputTruncatedFailsAndKeepsPolynomial) {
    std::istringstream input("3 1 2");
    Polynomial p = make({9});
    input >> p;
    CHECK(input.fail());
    CHECK(p == make({9}));
}

TEST(InputNotNumberFails) {
    std::istringstream input("abc");
    Polynomial p;
    input >> p;
    CHECK(input.fail());
}

TEST(EvaluateHighDegreeTerm) {
    Polynomial p(3, {0, 0, 0, 1});
    CHECK_EQUAL(8.0, p(2.0));
}

TEST(AddZeroKeepsPolynomial) {
    Polynomial p = make({1, 2, 3});
    CHECK(p + Polynomial() == p);
    CHECK(Polynomial() + p == p);
}

TEST(MultiplyByOneKeepsPolynomial) {
    Polynomial p = make({1, 2, 3});
    CHECK(p * make({1}) == p);
}

TEST(MultiplicationIsCommutative) {
    Polynomial a = make({1, 2, 3});
    Polynomial b = make({4, 5});
    CHECK(a * b == b * a);
}

TEST(ProductDegreeIsSumOfDegrees) {
    Polynomial product = make({1, 2, 3}) * make({1, 0, 0, 4});
    CHECK_EQUAL(5, product.degree());
}

TEST(CubeOfBinomial) {
    Polynomial p = make({1, 1});
    Polynomial cube = p;
    cube *= p;
    cube *= p;
    CHECK(cube == make({1, 3, 3, 1}));
}

TEST(DivideDifferenceOfCubes) {
    Polynomial quotient = make({-1, 0, 0, 1}) / make({-1, 1});
    CHECK(quotient == make({1, 1, 1}));
}

TEST(DivideByItselfGivesOne) {
    Polynomial p = make({1, 2, 3});
    CHECK(p / p == make({1}));
    CHECK((p % p).isZero());
}

TEST(DivideWhenLeadingCoefficientIsNotOne) {
    Polynomial dividend = make({2, 0, 4});
    Polynomial divisor = make({0, 2});
    CHECK(dividend / divisor == make({0, 2}));
    CHECK(dividend % divisor == make({2}));
}

TEST(DivideZeroPolynomialGivesZero) {
    CHECK((Polynomial() / make({1, 1})).isZero());
    CHECK((Polynomial() % make({1, 1})).isZero());
}

TEST(OutputUnitCoefficientsAndConstant) {
    CHECK_EQUAL(std::string("x + 1"), toString(make({1, 1})));
    CHECK_EQUAL(std::string("-x"), toString(make({0, -1})));
}

TEST(InputTrimsTrailingZeroCoefficients) {
    std::istringstream input("2 1 0 0");
    Polynomial p;
    input >> p;
    CHECK(p == make({1}));
}

TEST(InputAcceptsFractions) {
    std::istringstream input("1 0.5 2");
    Polynomial p;
    input >> p;
    CHECK(p == make({0.5, 2}));
}

TEST(InputTwoPolynomialsFromOneStream) {
    std::istringstream input("1 1 1\n2 0 0 1\n");
    Polynomial first;
    Polynomial second;
    input >> first >> second;
    CHECK(!input.fail());
    CHECK(first == make({1, 1}));
    CHECK(second == make({0, 0, 1}));
}

int main() {
    return UnitTest::RunAllTests();
}
