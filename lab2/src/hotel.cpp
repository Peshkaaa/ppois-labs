#include "hotel/hotel.hpp"

#include <algorithm>

#include "hotel/exceptions.hpp"
#include "hotel/validation.hpp"

namespace hotel {

ShiftSchedule::ShiftSchedule(int maxShifts) : maxShifts_(maxShifts) { requirePositive(maxShifts_, "max shifts"); }

void ShiftSchedule::assign(const Employee& employee, const Date& day) {
    const int id = employee.getEmployeeId();
    if (isWorking(id, day)) {
        throw EmployeeNotAvailableException(employee.fullName() + " already works on " + day.label());
    }
    if (shiftsOf(id) >= maxShifts_) {
        throw EmployeeNotAvailableException(employee.fullName() + " reached the shift limit");
    }
    days_[id].push_back(day);
}

int ShiftSchedule::shiftsOf(int employeeId) const {
    const auto found = days_.find(employeeId);
    return found == days_.end() ? 0 : static_cast<int>(found->second.size());
}

bool ShiftSchedule::isWorking(int employeeId, const Date& day) const {
    const auto found = days_.find(employeeId);
    return found != days_.end() && std::find(found->second.begin(), found->second.end(), day) != found->second.end();
}

void ShiftSchedule::cancel(const Employee& employee, const Date& day) {
    if (!isWorking(employee.getEmployeeId(), day)) {
        throw EmployeeNotAvailableException(employee.fullName() + " has no shift on " + day.label());
    }
    auto& shifts = days_[employee.getEmployeeId()];
    shifts.erase(std::find(shifts.begin(), shifts.end(), day));
}

Hotel::Hotel(std::string name, Address address, CancellationPolicy policy)
    : name_(std::move(name)), address_(std::move(address)), policy_(policy), nextReservationId_(1) {
    requireText(name_, "hotel name");
}

void Hotel::addFloor(const Floor& floor) { floors_.push_back(floor); }

void Hotel::registerGuest(const std::shared_ptr<Guest>& guest) {
    requireNotNull(guest, "guest");
    guests_.push_back(guest);
}

std::shared_ptr<Guest> Hotel::findGuest(int guestId) const {
    const auto found = std::find_if(guests_.begin(), guests_.end(),
                                    [&](const std::shared_ptr<Guest>& guest) { return guest->getGuestId() == guestId; });
    if (found == guests_.end()) {
        throw GuestNotFoundException(guestId);
    }
    return *found;
}

std::vector<std::shared_ptr<Room>> Hotel::allRooms() const {
    std::vector<std::shared_ptr<Room>> rooms;
    for (const Floor& floor : floors_) {
        rooms.insert(rooms.end(), floor.getRooms().begin(), floor.getRooms().end());
    }
    return rooms;
}

std::shared_ptr<Room> Hotel::findRoom(int roomNumber) const {
    const auto rooms = allRooms();
    const auto found = std::find_if(rooms.begin(), rooms.end(),
                                    [&](const std::shared_ptr<Room>& room) { return room->getNumber() == roomNumber; });
    if (found == rooms.end()) {
        throw RoomNotFoundException(roomNumber);
    }
    return *found;
}

bool Hotel::isRoomFree(const Room& room, const DateRange& range) const {
    return std::none_of(reservations_.begin(), reservations_.end(), [&](const std::shared_ptr<Reservation>& item) {
        return item->isActive() && item->getRoom()->getNumber() == room.getNumber() && item->overlapsWith(range);
    });
}

std::shared_ptr<Room> Hotel::findFreeRoom(int guests, const DateRange& range) const {
    for (const auto& room : allRooms()) {
        if (room->getType()->canHost(guests) && isRoomFree(*room, range)) {
            return room;
        }
    }
    throw RoomNotAvailableException(std::string("no room for the requested dates"));
}

std::shared_ptr<Reservation> Hotel::book(int guestId, int guestCount, const DateRange& range) {
    const auto guest = findGuest(guestId);
    const auto room = findFreeRoom(guestCount, range);
    auto reservation = std::make_shared<Reservation>(nextReservationId_, guest, room, range, guestCount);
    ++nextReservationId_;
    reservations_.push_back(reservation);
    return reservation;
}

std::shared_ptr<Reservation> Hotel::findReservation(int reservationId) const {
    const auto found = std::find_if(reservations_.begin(), reservations_.end(),
                                    [&](const std::shared_ptr<Reservation>& item) { return item->getId() == reservationId; });
    if (found == reservations_.end()) {
        throw ReservationNotFoundException(reservationId);
    }
    return *found;
}

Money Hotel::cancelReservation(int reservationId, const Date& today) {
    return findReservation(reservationId)->cancel(today, policy_);
}

int Hotel::roomCount() const { return static_cast<int>(allRooms().size()); }

OccupancyReport::OccupancyReport(std::shared_ptr<Hotel> hotel, Date date) : hotel_(std::move(hotel)), date_(date) {
    requireNotNull(hotel_, "hotel");
}

int OccupancyReport::bookedRooms() const {
    const auto& reservations = hotel_->getReservations();
    return static_cast<int>(std::count_if(reservations.begin(), reservations.end(), [&](const auto& item) {
        return item->isActive() && item->getStay().contains(date_);
    }));
}

int OccupancyReport::occupancyPercent() const {
    const int total = hotel_->roomCount();
    return total == 0 ? 0 : bookedRooms() * constants::kPercentBase / total;
}

Money OccupancyReport::expectedRevenue() const {
    Money sum;
    for (const auto& item : hotel_->getReservations()) {
        sum = item->isActive() ? sum.add(item->totalCost()) : sum;
    }
    return sum;
}

}  // namespace hotel
