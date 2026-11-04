CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -pedantic
TEST_FLAGS =
TEST_LIBS = -lUnitTest++
PROGRAM = polynomial_calc

all: $(PROGRAM)

$(PROGRAM): main.cpp polynomial.cpp polynomial.h
	$(CXX) $(CXXFLAGS) main.cpp polynomial.cpp -o $(PROGRAM)

tests: tests.cpp polynomial.cpp polynomial.h
	$(CXX) $(CXXFLAGS) $(TEST_FLAGS) tests.cpp polynomial.cpp -o tests $(TEST_LIBS)

test: tests
	./tests

# Тесты с замером покрытия строк класса (должно быть больше 90%).
coverage:
	$(CXX) -std=c++17 -O0 --coverage $(TEST_FLAGS) tests.cpp polynomial.cpp -o tests_cov $(TEST_LIBS)
	./tests_cov
	gcov -n tests_cov-polynomial.gcda | grep -A1 "File 'polynomial.cpp'"

docs:
	doxygen Doxyfile

clean:
	rm -rf $(PROGRAM) tests tests_cov docs *.gcno *.gcda *.gcov

.PHONY: all test coverage docs clean
