#pragma once

#include <memory>
#include <string>

#include "hotel/exceptions.hpp"

namespace hotel {

void requireText(const std::string& value, const std::string& fieldName);
void requirePositive(long long value, const std::string& fieldName);
void requireNonNegative(long long value, const std::string& fieldName);

template <typename T>
void requireNotNull(const std::shared_ptr<T>& pointer, const std::string& fieldName) {
    if (!pointer) {
        throw InvalidDataException(fieldName + " must not be null");
    }
}

}  // namespace hotel
