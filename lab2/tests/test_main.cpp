#include "test_framework.hpp"

int main() {
    for (const auto& test : testing::registry()) {
        const int before = testing::failureCount();
        test.body();
        std::cout << (testing::failureCount() == before ? "[ OK ] " : "[FAIL] ") << test.name << "\n";
    }
    std::cout << testing::registry().size() << " tests, " << testing::failureCount() << " failed checks\n";
    return testing::failureCount() == 0 ? 0 : 1;
}
