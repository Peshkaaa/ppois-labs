#pragma once

#include <string>

#include "hotel/constants.hpp"

namespace hotel {

class Money {
public:
    explicit Money(long long cents = 0);
    long long getCents() const { return cents_; }
    Money add(const Money& other) const;
    Money subtract(const Money& other) const;
    Money multiply(int factor) const;
    Money percentOf(int percent) const;
    bool isZero() const;
    bool isNegative() const;
    std::string format() const;
    auto operator<=>(const Money& other) const = default;
    bool operator==(const Money& other) const = default;

private:
    long long cents_;
};

class Date {
public:
    explicit Date(int dayNumber = 0);
    int getDayNumber() const { return dayNumber_; }
    Date plusDays(int days) const;
    int daysTo(const Date& other) const;
    bool isBefore(const Date& other) const;
    bool isAfter(const Date& other) const;
    std::string label() const;
    auto operator<=>(const Date& other) const = default;
    bool operator==(const Date& other) const = default;

private:
    int dayNumber_;
};

class DateRange {
public:
    DateRange(Date start, Date end);
    const Date& getStart() const { return start_; }
    const Date& getEnd() const { return end_; }
    int nights() const;
    bool overlaps(const DateRange& other) const;
    bool contains(const Date& day) const;

private:
    Date start_;
    Date end_;
};

class Address {
public:
    Address(std::string street, std::string city, std::string country, std::string postalCode);
    const std::string& getCity() const { return city_; }
    const std::string& getCountry() const { return country_; }
    std::string formatted() const;
    bool sameCity(const Address& other) const;

private:
    std::string street_;
    std::string city_;
    std::string country_;
    std::string postalCode_;
};

class ContactInfo {
public:
    ContactInfo(std::string phone, std::string email);
    const std::string& getPhone() const { return phone_; }
    const std::string& getEmail() const { return email_; }
    std::string emailDomain() const;
    std::string maskedPhone() const;
    bool sameEmailDomain(const ContactInfo& other) const;

private:
    std::string phone_;
    std::string email_;
};

}  // namespace hotel
