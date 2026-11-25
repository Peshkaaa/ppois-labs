#include "hotel/validation.hpp"

namespace hotel {

void requireText(const std::string& value, const std::string& fieldName) {
    if (value.empty()) {
        throw InvalidDataException(fieldName + " must not be empty");
    }
}

void requirePositive(long long value, const std::string& fieldName) {
    if (value <= 0) {
        throw InvalidDataException(fieldName + " must be positive");
    }
}

void requireNonNegative(long long value, const std::string& fieldName) {
    if (value < 0) {
        throw InvalidDataException(fieldName + " must not be negative");
    }
}

}  // namespace hotel
