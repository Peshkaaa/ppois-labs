#include "hotel/housekeeping.hpp"

#include "hotel/exceptions.hpp"
#include "hotel/validation.hpp"

namespace hotel {

InventoryItem::InventoryItem(std::string name, int quantity, int minimumLevel)
    : name_(std::move(name)), quantity_(quantity), minimumLevel_(minimumLevel) {
    requireText(name_, "inventory name");
    requireNonNegative(quantity_, "quantity");
    requireNonNegative(minimumLevel_, "minimum level");
}

void InventoryItem::consume(int amount) {
    requirePositive(amount, "amount");
    if (amount > quantity_) {
        throw InsufficientFundsException("stock of " + name_);
    }
    quantity_ -= amount;
}

void InventoryItem::restock(int amount) {
    requirePositive(amount, "amount");
    quantity_ += amount;
}

bool InventoryItem::needsRestock() const { return quantity_ <= minimumLevel_; }

Housekeeper::Housekeeper(Employee base, int assignedFloor)
    : Employee(std::move(base)), assignedFloor_(assignedFloor), tasksDone_(0) {
    requirePositive(assignedFloor_, "assigned floor");
}

void Housekeeper::assignFloor(int floor) {
    requirePositive(floor, "floor");
    assignedFloor_ = floor;
}

void Housekeeper::recordCompletedTask() { ++tasksDone_; }

bool Housekeeper::isAssignedTo(int floor) const { return assignedFloor_ == floor; }

CleaningTask::CleaningTask(std::shared_ptr<Room> room, std::shared_ptr<Housekeeper> housekeeper)
    : room_(std::move(room)), housekeeper_(std::move(housekeeper)), done_(false) {
    requireNotNull(room_, "room");
    requireNotNull(housekeeper_, "housekeeper");
}

void CleaningTask::complete() {
    if (done_) {
        throw InvalidStatusTransitionException("Completed", "Completed");
    }
    if (!housekeeper_->isOnDuty()) {
        throw EmployeeNotAvailableException(housekeeper_->fullName() + " is not on duty");
    }
    room_->markAvailable();
    housekeeper_->recordCompletedTask();
    done_ = true;
}

Technician::Technician(Employee base, std::string specialty)
    : Employee(std::move(base)), specialty_(std::move(specialty)), requestsClosed_(0) {
    requireText(specialty_, "specialty");
}

bool Technician::canHandle(const std::string& category) const { return specialty_ == category; }

void Technician::recordClosedRequest() { ++requestsClosed_; }

MaintenanceRequest::MaintenanceRequest(int id, std::shared_ptr<Room> room, std::string description)
    : id_(id), room_(std::move(room)), description_(std::move(description)), resolved_(false), cost_(Money(0)) {
    requireNotNull(room_, "room");
    requireText(description_, "description");
    room_->sendToMaintenance();
}

void MaintenanceRequest::assign(const std::shared_ptr<Technician>& technician) {
    requireNotNull(technician, "technician");
    if (!technician->isOnDuty()) {
        throw EmployeeNotAvailableException(technician->fullName() + " is not on duty");
    }
    technician_ = technician;
}

void MaintenanceRequest::resolve(const Money& cost) {
    if (!technician_) {
        throw EmployeeNotAvailableException("no technician assigned");
    }
    if (resolved_) {
        throw InvalidStatusTransitionException("Resolved", "Resolved");
    }
    room_->markAvailable();
    technician_->recordClosedRequest();
    cost_ = cost;
    resolved_ = true;
}

LostItem::LostItem(std::string description, std::shared_ptr<Room> foundInRoom, Date foundDate)
    : description_(std::move(description)), foundInRoom_(std::move(foundInRoom)), foundDate_(foundDate),
      claimed_(false) {
    requireText(description_, "description");
    requireNotNull(foundInRoom_, "room");
}

void LostItem::claim() {
    if (claimed_) {
        throw InvalidStatusTransitionException("Claimed", "Claimed");
    }
    claimed_ = true;
}

int LostItem::daysStored(const Date& today) const { return foundDate_.daysTo(today); }

bool LostItem::isExpired(const Date& today) const { return daysStored(today) > constants::kLostItemStorageDays; }

}  // namespace hotel
