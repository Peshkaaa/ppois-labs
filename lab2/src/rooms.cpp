#include "hotel/rooms.hpp"

#include <algorithm>
#include <iterator>
#include <numeric>

#include "hotel/exceptions.hpp"
#include "hotel/validation.hpp"

namespace hotel {

Amenity::Amenity(std::string name, Money dailyCharge) : name_(std::move(name)), dailyCharge_(dailyCharge) {
    requireText(name_, "amenity name");
    requireNonNegative(dailyCharge_.getCents(), "amenity charge");
}

bool Amenity::isComplimentary() const { return dailyCharge_.isZero(); }

Money Amenity::chargeForNights(int nights) const { return dailyCharge_.multiply(nights); }

std::string Amenity::describe() const { return name_ + " (" + dailyCharge_.format() + "/day)"; }

Bed::Bed(BedKind kind, int capacity) : kind_(kind), capacity_(capacity) { requirePositive(capacity_, "bed capacity"); }

bool Bed::fitsGuests(int guests) const { return guests > 0 && guests <= capacity_; }

std::string Bed::kindName() const {
    switch (kind_) {
        case BedKind::King:
            return "King";
        case BedKind::Double:
            return "Double";
        default:
            return "Single";
    }
}

RoomType::RoomType(std::string name, int capacity, Money baseRate)
    : name_(std::move(name)), capacity_(capacity), baseRate_(baseRate) {
    requireText(name_, "room type name");
    requirePositive(capacity_, "room capacity");
    requireNonNegative(baseRate_.getCents(), "base rate");
}

void RoomType::addBed(const Bed& bed) { beds_.push_back(bed); }

void RoomType::addAmenity(const Amenity& amenity) { amenities_.push_back(amenity); }

int RoomType::totalBedCapacity() const {
    return std::accumulate(beds_.begin(), beds_.end(), 0, [](int sum, const Bed& bed) { return sum + bed.getCapacity(); });
}

Money RoomType::amenitiesDailyCost() const {
    Money total;
    for (const Amenity& amenity : amenities_) {
        total = total.add(amenity.getDailyCharge());
    }
    return total;
}

bool RoomType::hasAmenity(const std::string& name) const {
    return std::any_of(amenities_.begin(), amenities_.end(), [&](const Amenity& item) { return item.getName() == name; });
}

bool RoomType::canHost(int guests) const { return guests > 0 && guests <= capacity_; }

Money RoomType::nightlyRate() const { return baseRate_.add(amenitiesDailyCost()); }

Room::Room(int number, int floor, std::shared_ptr<RoomType> type)
    : number_(number), floor_(floor), type_(std::move(type)), status_(RoomStatus::Available) {
    requirePositive(number_, "room number");
    requirePositive(floor_, "floor");
    requireNotNull(type_, "room type");
}

bool Room::isAvailable() const { return status_ == RoomStatus::Available; }

void Room::occupy() {
    if (!isAvailable()) {
        throw RoomNotAvailableException(number_);
    }
    status_ = RoomStatus::Occupied;
}

void Room::release() {
    if (status_ != RoomStatus::Occupied) {
        throw InvalidStatusTransitionException(statusName(), "Cleaning");
    }
    status_ = RoomStatus::Cleaning;
}

void Room::markAvailable() {
    if (status_ != RoomStatus::Cleaning && status_ != RoomStatus::Maintenance) {
        throw InvalidStatusTransitionException(statusName(), "Available");
    }
    status_ = RoomStatus::Available;
}

void Room::sendToMaintenance() {
    if (status_ == RoomStatus::Occupied) {
        throw InvalidStatusTransitionException(statusName(), "Maintenance");
    }
    status_ = RoomStatus::Maintenance;
}

Money Room::ratePerNight() const { return type_->nightlyRate(); }

std::string Room::statusName() const {
    switch (status_) {
        case RoomStatus::Occupied:
            return "Occupied";
        case RoomStatus::Cleaning:
            return "Cleaning";
        case RoomStatus::Maintenance:
            return "Maintenance";
        default:
            return "Available";
    }
}

Floor::Floor(int number) : number_(number) { requirePositive(number_, "floor number"); }

void Floor::addRoom(const std::shared_ptr<Room>& room) {
    requireNotNull(room, "room");
    if (room->getFloor() != number_) {
        throw InvalidDataException("room belongs to another floor");
    }
    rooms_.push_back(room);
}

std::shared_ptr<Room> Floor::findRoom(int roomNumber) const {
    const auto found = std::find_if(rooms_.begin(), rooms_.end(),
                                    [&](const std::shared_ptr<Room>& room) { return room->getNumber() == roomNumber; });
    if (found == rooms_.end()) {
        throw RoomNotFoundException(roomNumber);
    }
    return *found;
}

std::vector<std::shared_ptr<Room>> Floor::availableRooms() const {
    std::vector<std::shared_ptr<Room>> result;
    std::copy_if(rooms_.begin(), rooms_.end(), std::back_inserter(result),
                 [](const std::shared_ptr<Room>& room) { return room->isAvailable(); });
    return result;
}

int Floor::occupiedCount() const {
    return static_cast<int>(std::count_if(rooms_.begin(), rooms_.end(), [](const std::shared_ptr<Room>& room) {
        return room->getStatus() == RoomStatus::Occupied;
    }));
}

KeyCard::KeyCard(std::string cardId, std::shared_ptr<Room> room, DateRange validity)
    : cardId_(std::move(cardId)), room_(std::move(room)), validity_(validity), active_(true) {
    requireText(cardId_, "key card id");
    requireNotNull(room_, "room");
}

bool KeyCard::isValidOn(const Date& day) const { return active_ && validity_.contains(day); }

bool KeyCard::opens(const Room& room, const Date& day) const {
    return isValidOn(day) && room.getNumber() == room_->getNumber();
}

void KeyCard::deactivate() { active_ = false; }

void KeyCard::extend(const Date& newEnd) { validity_ = DateRange(validity_.getStart(), newEnd); }

}  // namespace hotel
