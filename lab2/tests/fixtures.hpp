#pragma once

#include <memory>

#include "hotel/billing.hpp"
#include "hotel/exceptions.hpp"
#include "hotel/validation.hpp"
#include "hotel/booking.hpp"
#include "hotel/dining.hpp"
#include "hotel/facilities.hpp"
#include "hotel/feedback.hpp"
#include "hotel/hotel.hpp"
#include "hotel/housekeeping.hpp"

namespace fixtures {

using namespace hotel;

inline ContactInfo contact() { return ContactInfo("+375291234567", "user@mail.test"); }

inline Person person(const std::string& first = "Ivan") { return Person(first, "Ivanov", 30, contact()); }

inline std::shared_ptr<Guest> guest(int id = 1) {
    return std::make_shared<Guest>(person(), id, PassportData("AB1234567", "BY", Date(1000)),
                                   std::make_shared<LoyaltyCard>("LC-" + std::to_string(id)));
}

inline Employee employee(int id = 1) { return Employee(person("Petr"), id, "Staff", Money(100000), Date(0)); }

inline std::shared_ptr<Receptionist> receptionist(bool onDuty = true) {
    auto result = std::make_shared<Receptionist>(employee(10), 1);
    if (onDuty) result->startShift();
    return result;
}

inline std::shared_ptr<Housekeeper> housekeeper(bool onDuty = true) {
    auto result = std::make_shared<Housekeeper>(employee(11), 1);
    if (onDuty) result->startShift();
    return result;
}

inline std::shared_ptr<Technician> technician(bool onDuty = true) {
    auto result = std::make_shared<Technician>(employee(12), "plumbing");
    if (onDuty) result->startShift();
    return result;
}

inline std::shared_ptr<Waiter> waiter() { return std::make_shared<Waiter>(employee(13)); }

inline std::shared_ptr<RoomType> roomType(int capacity = 2) {
    auto type = std::make_shared<RoomType>("Standard", capacity, Money(10000));
    type->addBed(Bed(BedKind::Double, capacity));
    type->addAmenity(Amenity("Wifi", Money(500)));
    return type;
}

inline std::shared_ptr<Room> room(int number = 101) { return std::make_shared<Room>(number, 1, roomType()); }

inline DateRange range(int from = 10, int to = 12) { return DateRange(Date(from), Date(to)); }

inline std::shared_ptr<Reservation> reservation(int id = 1, int from = 10, int to = 12) {
    return std::make_shared<Reservation>(id, guest(), room(), range(from, to), 2);
}

inline std::shared_ptr<Hotel> hotelWithRooms() {
    auto result = std::make_shared<Hotel>("H", Address("S", "C", "K", "1"), CancellationPolicy(3, 50));
    Floor floor(1);
    floor.addRoom(room(101));
    floor.addRoom(room(102));
    result->addFloor(floor);
    result->registerGuest(guest(1));
    return result;
}

}  // namespace fixtures
