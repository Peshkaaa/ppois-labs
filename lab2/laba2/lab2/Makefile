CXX ?= g++
CXXFLAGS ?= -std=c++20 -Wall -Wextra -Wpedantic -Iinclude -Icli
BUILD := build
LIB_SRC := $(wildcard src/*.cpp)
CLI_SRC := $(wildcard cli/*.cpp)
TEST_SRC := $(wildcard tests/*.cpp)

.PHONY: all test clean

all: $(BUILD)/hotel_cli

$(BUILD)/hotel_cli: $(LIB_SRC) $(CLI_SRC) app/main.cpp
	@mkdir -p $(BUILD)
	$(CXX) $(CXXFLAGS) -O2 $^ -o $@

$(BUILD)/hotel_tests: $(LIB_SRC) $(CLI_SRC) $(TEST_SRC)
	@mkdir -p $(BUILD)
	$(CXX) $(CXXFLAGS) -O0 --coverage -Itests $^ -o $@

test: $(BUILD)/hotel_tests
	./$(BUILD)/hotel_tests

clean:
	rm -rf $(BUILD)
