#pragma once

#include <functional>
#include <iostream>
#include <string>
#include <vector>

namespace testing {

struct TestCase {
    std::string name;
    std::function<void()> body;
};

inline std::vector<TestCase>& registry() {
    static std::vector<TestCase> tests;
    return tests;
}

inline int& failureCount() {
    static int failures = 0;
    return failures;
}

struct Registrar {
    Registrar(const std::string& name, std::function<void()> body) { registry().push_back({name, std::move(body)}); }
};

inline void reportFailure(const char* expression, const char* file, int line) {
    ++failureCount();
    std::cerr << "  FAILED: " << expression << " (" << file << ":" << line << ")\n";
}

}  // namespace testing

#define TEST_CASE(name)                                         \
    static void name();                                         \
    static testing::Registrar registrar_##name(#name, name);    \
    static void name()

#define CHECK(condition)                                                     \
    do {                                                                     \
        if (!(condition)) testing::reportFailure(#condition, __FILE__, __LINE__); \
    } while (false)

#define CHECK_THROWS_AS(expression, ExceptionType)                                   \
    do {                                                                             \
        bool thrown = false;                                                         \
        try {                                                                        \
            (void)(expression);                                                      \
        } catch (const ExceptionType&) {                                             \
            thrown = true;                                                           \
        } catch (...) {                                                              \
        }                                                                            \
        if (!thrown) testing::reportFailure(#expression " throws " #ExceptionType, __FILE__, __LINE__); \
    } while (false)
