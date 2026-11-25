#pragma once

#include <memory>
#include <string>
#include <vector>

#include "hotel/common.hpp"

namespace hotel {

class Person {
public:
    Person(std::string firstName, std::string lastName, int age, ContactInfo contact);
    virtual ~Person() = default;
    const std::string& getFirstName() const { return firstName_; }
    const std::string& getLastName() const { return lastName_; }
    int getAge() const { return age_; }
    const ContactInfo& getContact() const { return contact_; }
    std::string fullName() const;
    bool isAdult() const;
    std::string initials() const;
    virtual std::string describe() const;

private:
    std::string firstName_;
    std::string lastName_;
    int age_;
    ContactInfo contact_;
};

class PassportData {
public:
    PassportData(std::string number, std::string country, Date expiry);
    const std::string& getNumber() const { return number_; }
    const Date& getExpiry() const { return expiry_; }
    bool isExpiredOn(const Date& day) const;
    void ensureValidOn(const Date& day) const;
    std::string maskedNumber() const;

private:
    std::string number_;
    std::string country_;
    Date expiry_;
};

enum class LoyaltyTier { Standard, Silver, Gold };

class LoyaltyCard {
public:
    explicit LoyaltyCard(std::string cardNumber);
    const std::string& getCardNumber() const { return cardNumber_; }
    int getPoints() const { return points_; }
    LoyaltyTier getTier() const { return tier_; }
    void earnPoints(int amount);
    void redeemPoints(int amount);
    int discountPercent() const;
    std::string tierName() const;

private:
    void refreshTier();

    std::string cardNumber_;
    int points_;
    LoyaltyTier tier_;
};

class Guest : public Person {
public:
    Guest(Person base, int guestId, PassportData passport, std::shared_ptr<LoyaltyCard> loyaltyCard);
    int getGuestId() const { return guestId_; }
    const PassportData& getPassport() const { return passport_; }
    const std::shared_ptr<LoyaltyCard>& getLoyaltyCard() const { return loyaltyCard_; }
    const std::vector<std::string>& getPreferences() const { return preferences_; }
    void addPreference(const std::string& preference);
    bool hasPreference(const std::string& preference) const;
    bool isVip() const;
    void earnPointsForStay(int nights);
    std::string describe() const override;

private:
    int guestId_;
    PassportData passport_;
    std::shared_ptr<LoyaltyCard> loyaltyCard_;
    std::vector<std::string> preferences_;
};

class Employee : public Person {
public:
    Employee(Person base, int employeeId, std::string position, Money salary, Date hireDate);
    int getEmployeeId() const { return employeeId_; }
    const std::string& getPosition() const { return position_; }
    const Money& getSalary() const { return salary_; }
    const Date& getHireDate() const { return hireDate_; }
    bool isOnDuty() const { return onDuty_; }
    void raiseSalary(int percent);
    void startShift();
    void endShift();
    int daysEmployed(const Date& today) const;
    std::string describe() const override;

private:
    int employeeId_;
    std::string position_;
    Money salary_;
    Date hireDate_;
    bool onDuty_;
};

class Receptionist : public Employee {
public:
    Receptionist(Employee base, int deskNumber);
    int getDeskNumber() const { return deskNumber_; }
    bool speaksLanguage(const std::string& language) const;
    void addLanguage(const std::string& language);
    void assignDesk(int deskNumber);

private:
    std::vector<std::string> languages_;
    int deskNumber_;
};

class Manager : public Employee {
public:
    Manager(Employee base, std::string department, Money bonusBudget);
    const std::string& getDepartment() const { return department_; }
    const Money& getBonusBudget() const { return bonusBudget_; }
    void addSubordinate(const std::shared_ptr<Employee>& employee);
    int teamSize() const;
    bool manages(const Employee& employee) const;
    Money awardBonus(const Employee& employee, const Money& bonus);

private:
    std::string department_;
    Money bonusBudget_;
    std::vector<std::shared_ptr<Employee>> subordinates_;
};

}  // namespace hotel
