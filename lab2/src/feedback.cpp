#include "hotel/feedback.hpp"

#include "hotel/exceptions.hpp"
#include "hotel/validation.hpp"

namespace hotel {

Review::Review(std::shared_ptr<Guest> guest, int rating, std::string text, Date date)
    : guest_(std::move(guest)), rating_(rating), text_(std::move(text)), date_(date) {
    requireNotNull(guest_, "guest");
    if (rating_ < constants::kMinRating || rating_ > constants::kMaxRating) {
        throw InvalidDataException("rating out of range");
    }
}

bool Review::isPositive() const { return rating_ >= constants::kPositiveRating; }

std::string Review::summary() const {
    return guest_->fullName() + ": " + std::to_string(rating_) + "/" + std::to_string(constants::kMaxRating);
}

Notification::Notification(std::shared_ptr<Guest> recipient, std::string message)
    : recipient_(std::move(recipient)), message_(std::move(message)), sent_(false) {
    requireNotNull(recipient_, "recipient");
    requireText(message_, "message");
}

void Notification::send() {
    if (sent_) {
        throw InvalidStatusTransitionException("Sent", "Sent");
    }
    sent_ = true;
}

std::string Notification::preview() const {
    const std::size_t limit = constants::kPreviewLength;
    return message_.size() <= limit ? message_ : message_.substr(0, limit) + "...";
}

Complaint::Complaint(std::shared_ptr<Guest> guest, std::string description)
    : guest_(std::move(guest)), description_(std::move(description)), resolved_(false), compensation_(Money(0)) {
    requireNotNull(guest_, "guest");
    requireText(description_, "description");
}

void Complaint::escalateTo(const std::shared_ptr<Manager>& manager) {
    requireNotNull(manager, "manager");
    manager_ = manager;
}

bool Complaint::needsEscalation() const { return !resolved_ && !manager_; }

void Complaint::resolve(const Money& compensation) {
    if (resolved_) {
        throw InvalidStatusTransitionException("Resolved", "Resolved");
    }
    if (!compensation.isZero() && !manager_) {
        throw EmployeeNotAvailableException("manager approval required for compensation");
    }
    compensation_ = compensation;
    resolved_ = true;
}

}  // namespace hotel
