#include "hotel/people.hpp"

#include <algorithm>

#include "hotel/exceptions.hpp"
#include "hotel/validation.hpp"

namespace hotel {

Person::Person(std::string firstName, std::string lastName, int age, ContactInfo contact)
    : firstName_(std::move(firstName)), lastName_(std::move(lastName)), age_(age), contact_(std::move(contact)) {
    requireText(firstName_, "first name");
    requireText(lastName_, "last name");
    requireNonNegative(age_, "age");
}

std::string Person::fullName() const { return firstName_ + " " + lastName_; }

bool Person::isAdult() const { return age_ >= constants::kAdultAge; }

std::string Person::initials() const { return std::string(1, firstName_.front()) + std::string(1, lastName_.front()); }

std::string Person::describe() const { return "Person " + fullName(); }

PassportData::PassportData(std::string number, std::string country, Date expiry)
    : number_(std::move(number)), country_(std::move(country)), expiry_(expiry) {
    requireText(number_, "passport number");
    requireText(country_, "passport country");
}

bool PassportData::isExpiredOn(const Date& day) const { return expiry_.isBefore(day); }

void PassportData::ensureValidOn(const Date& day) const {
    if (isExpiredOn(day)) {
        throw ExpiredDocumentException("passport " + maskedNumber());
    }
}

std::string PassportData::maskedNumber() const {
    const std::size_t visible = std::min<std::size_t>(constants::kVisibleDigits, number_.size() / constants::kHalf);
    return std::string(number_.size() - visible, '*') + number_.substr(number_.size() - visible);
}

LoyaltyCard::LoyaltyCard(std::string cardNumber)
    : cardNumber_(std::move(cardNumber)), points_(0), tier_(LoyaltyTier::Standard) {
    requireText(cardNumber_, "card number");
}

void LoyaltyCard::earnPoints(int amount) {
    requirePositive(amount, "points");
    points_ += amount;
    refreshTier();
}

void LoyaltyCard::redeemPoints(int amount) {
    requirePositive(amount, "points");
    if (amount > points_) {
        throw InsufficientFundsException("loyalty points");
    }
    points_ -= amount;
    refreshTier();
}

int LoyaltyCard::discountPercent() const {
    switch (tier_) {
        case LoyaltyTier::Gold:
            return constants::kGoldDiscount;
        case LoyaltyTier::Silver:
            return constants::kSilverDiscount;
        default:
            return constants::kStandardDiscount;
    }
}

std::string LoyaltyCard::tierName() const {
    switch (tier_) {
        case LoyaltyTier::Gold:
            return "Gold";
        case LoyaltyTier::Silver:
            return "Silver";
        default:
            return "Standard";
    }
}

void LoyaltyCard::refreshTier() {
    if (points_ >= constants::kGoldThreshold) {
        tier_ = LoyaltyTier::Gold;
    } else if (points_ >= constants::kSilverThreshold) {
        tier_ = LoyaltyTier::Silver;
    } else {
        tier_ = LoyaltyTier::Standard;
    }
}

Guest::Guest(Person base, int guestId, PassportData passport, std::shared_ptr<LoyaltyCard> loyaltyCard)
    : Person(std::move(base)), guestId_(guestId), passport_(std::move(passport)), loyaltyCard_(std::move(loyaltyCard)) {
    requirePositive(guestId_, "guest id");
    requireNotNull(loyaltyCard_, "loyalty card");
}

void Guest::addPreference(const std::string& preference) {
    requireText(preference, "preference");
    if (!hasPreference(preference)) {
        preferences_.push_back(preference);
    }
}

bool Guest::hasPreference(const std::string& preference) const {
    return std::find(preferences_.begin(), preferences_.end(), preference) != preferences_.end();
}

bool Guest::isVip() const { return loyaltyCard_->getTier() == LoyaltyTier::Gold; }

void Guest::earnPointsForStay(int nights) {
    requirePositive(nights, "nights");
    loyaltyCard_->earnPoints(nights * constants::kPointsPerNight);
}

std::string Guest::describe() const { return "Guest " + fullName() + " #" + std::to_string(guestId_); }

Employee::Employee(Person base, int employeeId, std::string position, Money salary, Date hireDate)
    : Person(std::move(base)), employeeId_(employeeId), position_(std::move(position)), salary_(salary),
      hireDate_(hireDate), onDuty_(false) {
    requirePositive(employeeId_, "employee id");
    requireText(position_, "position");
    requireNonNegative(salary_.getCents(), "salary");
}

void Employee::raiseSalary(int percent) {
    requirePositive(percent, "raise percent");
    salary_ = salary_.add(salary_.percentOf(percent));
}

void Employee::startShift() {
    if (onDuty_) {
        throw EmployeeNotAvailableException(fullName() + " is already on duty");
    }
    onDuty_ = true;
}

void Employee::endShift() {
    if (!onDuty_) {
        throw InvalidStatusTransitionException("off duty", "off duty");
    }
    onDuty_ = false;
}

int Employee::daysEmployed(const Date& today) const { return hireDate_.daysTo(today); }

std::string Employee::describe() const { return "Employee " + fullName() + " (" + position_ + ")"; }

Receptionist::Receptionist(Employee base, int deskNumber) : Employee(std::move(base)), deskNumber_(deskNumber) {
    requirePositive(deskNumber_, "desk number");
}

bool Receptionist::speaksLanguage(const std::string& language) const {
    return std::find(languages_.begin(), languages_.end(), language) != languages_.end();
}

void Receptionist::addLanguage(const std::string& language) {
    requireText(language, "language");
    if (!speaksLanguage(language)) {
        languages_.push_back(language);
    }
}

void Receptionist::assignDesk(int deskNumber) {
    requirePositive(deskNumber, "desk number");
    deskNumber_ = deskNumber;
}

Manager::Manager(Employee base, std::string department, Money bonusBudget)
    : Employee(std::move(base)), department_(std::move(department)), bonusBudget_(bonusBudget) {
    requireText(department_, "department");
    requireNonNegative(bonusBudget_.getCents(), "bonus budget");
}

void Manager::addSubordinate(const std::shared_ptr<Employee>& employee) {
    requireNotNull(employee, "subordinate");
    if (!manages(*employee)) {
        subordinates_.push_back(employee);
    }
}

int Manager::teamSize() const { return static_cast<int>(subordinates_.size()); }

bool Manager::manages(const Employee& employee) const {
    return std::any_of(subordinates_.begin(), subordinates_.end(), [&](const std::shared_ptr<Employee>& member) {
        return member->getEmployeeId() == employee.getEmployeeId();
    });
}

Money Manager::awardBonus(const Employee& employee, const Money& bonus) {
    if (!manages(employee)) {
        throw EmployeeNotAvailableException(employee.fullName() + " is not in the team");
    }
    if (bonus > bonusBudget_) {
        throw InsufficientFundsException("bonus budget");
    }
    bonusBudget_ = bonusBudget_.subtract(bonus);
    return bonus;
}

}  // namespace hotel
