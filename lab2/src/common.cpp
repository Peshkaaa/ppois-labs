#include "hotel/common.hpp"

#include <algorithm>

#include "hotel/exceptions.hpp"
#include "hotel/validation.hpp"

namespace hotel {

Money::Money(long long cents) : cents_(cents) {}

Money Money::add(const Money& other) const { return Money(cents_ + other.cents_); }

Money Money::subtract(const Money& other) const { return Money(cents_ - other.cents_); }

Money Money::multiply(int factor) const { return Money(cents_ * factor); }

Money Money::percentOf(int percent) const { return Money(cents_ * percent / constants::kPercentBase); }

bool Money::isZero() const { return cents_ == 0; }

bool Money::isNegative() const { return cents_ < 0; }

std::string Money::format() const {
    const long long absolute = isNegative() ? -cents_ : cents_;
    const long long units = absolute / constants::kCentsPerUnit;
    const long long fraction = absolute % constants::kCentsPerUnit;
    std::string text = std::string(isNegative() ? "-" : "") + std::to_string(units) + ".";
    if (fraction < constants::kTwoDigitLimit) {
        text += "0";
    }
    return text + std::to_string(fraction);
}

Date::Date(int dayNumber) : dayNumber_(dayNumber) { requireNonNegative(dayNumber, "day number"); }

Date Date::plusDays(int days) const { return Date(dayNumber_ + days); }

int Date::daysTo(const Date& other) const { return other.dayNumber_ - dayNumber_; }

bool Date::isBefore(const Date& other) const { return dayNumber_ < other.dayNumber_; }

bool Date::isAfter(const Date& other) const { return dayNumber_ > other.dayNumber_; }

std::string Date::label() const { return "Day " + std::to_string(dayNumber_); }

DateRange::DateRange(Date start, Date end) : start_(start), end_(end) {
    if (!start_.isBefore(end_)) {
        throw InvalidDateRangeException(start_.getDayNumber(), end_.getDayNumber());
    }
}

int DateRange::nights() const { return start_.daysTo(end_); }

bool DateRange::overlaps(const DateRange& other) const {
    return start_.isBefore(other.end_) && other.start_.isBefore(end_);
}

bool DateRange::contains(const Date& day) const { return !day.isBefore(start_) && day.isBefore(end_); }

Address::Address(std::string street, std::string city, std::string country, std::string postalCode)
    : street_(std::move(street)), city_(std::move(city)), country_(std::move(country)),
      postalCode_(std::move(postalCode)) {
    requireText(street_, "street");
    requireText(city_, "city");
    requireText(country_, "country");
    requireText(postalCode_, "postal code");
}

std::string Address::formatted() const { return street_ + ", " + postalCode_ + " " + city_ + ", " + country_; }

bool Address::sameCity(const Address& other) const { return city_ == other.city_ && country_ == other.country_; }

ContactInfo::ContactInfo(std::string phone, std::string email) : phone_(std::move(phone)), email_(std::move(email)) {
    const auto at = email_.find('@');
    if (at == std::string::npos || at == 0 || at + 1 >= email_.size()) {
        throw InvalidDataException("email must contain local part and domain");
    }
    if (static_cast<int>(phone_.size()) < constants::kMinPhoneDigits) {
        throw InvalidDataException("phone is too short");
    }
}

std::string ContactInfo::emailDomain() const { return email_.substr(email_.find('@') + 1); }

std::string ContactInfo::maskedPhone() const {
    const std::string tail = phone_.substr(phone_.size() - constants::kVisibleDigits);
    return std::string(phone_.size() - tail.size(), '*') + tail;
}

bool ContactInfo::sameEmailDomain(const ContactInfo& other) const { return emailDomain() == other.emailDomain(); }

}  // namespace hotel
