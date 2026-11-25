#pragma once

#include <memory>
#include <string>

#include "hotel/people.hpp"

namespace hotel {

class Review {
public:
    Review(std::shared_ptr<Guest> guest, int rating, std::string text, Date date);
    int getRating() const { return rating_; }
    bool isPositive() const;
    std::string summary() const;

private:
    std::shared_ptr<Guest> guest_;
    int rating_;
    std::string text_;
    Date date_;
};

class Notification {
public:
    Notification(std::shared_ptr<Guest> recipient, std::string message);
    bool isSent() const { return sent_; }
    void send();
    std::string preview() const;

private:
    std::shared_ptr<Guest> recipient_;
    std::string message_;
    bool sent_;
};

class Complaint {
public:
    Complaint(std::shared_ptr<Guest> guest, std::string description);
    bool isOpen() const { return !resolved_; }
    const Money& getCompensation() const { return compensation_; }
    void escalateTo(const std::shared_ptr<Manager>& manager);
    bool needsEscalation() const;
    void resolve(const Money& compensation);

private:
    std::shared_ptr<Guest> guest_;
    std::string description_;
    bool resolved_;
    Money compensation_;
    std::shared_ptr<Manager> manager_;
};

}  // namespace hotel
