#pragma once

#include <map>
#include <memory>
#include <string>
#include <vector>

#include "hotel/booking.hpp"
#include "hotel/rooms.hpp"

namespace hotel {

class ShiftSchedule {
public:
    explicit ShiftSchedule(int maxShifts);
    void assign(const Employee& employee, const Date& day);
    int shiftsOf(int employeeId) const;
    bool isWorking(int employeeId, const Date& day) const;
    void cancel(const Employee& employee, const Date& day);

private:
    int maxShifts_;
    std::map<int, std::vector<Date>> days_;
};

class Hotel {
public:
    Hotel(std::string name, Address address, CancellationPolicy policy);
    const std::string& getName() const { return name_; }
    const std::vector<std::shared_ptr<Reservation>>& getReservations() const { return reservations_; }
    void addFloor(const Floor& floor);
    void registerGuest(const std::shared_ptr<Guest>& guest);
    std::shared_ptr<Guest> findGuest(int guestId) const;
    std::vector<std::shared_ptr<Room>> allRooms() const;
    std::shared_ptr<Room> findRoom(int roomNumber) const;
    bool isRoomFree(const Room& room, const DateRange& range) const;
    std::shared_ptr<Room> findFreeRoom(int guests, const DateRange& range) const;
    std::shared_ptr<Reservation> book(int guestId, int guestCount, const DateRange& range);
    std::shared_ptr<Reservation> findReservation(int reservationId) const;
    Money cancelReservation(int reservationId, const Date& today);
    int roomCount() const;

private:
    std::string name_;
    Address address_;
    CancellationPolicy policy_;
    std::vector<Floor> floors_;
    std::vector<std::shared_ptr<Guest>> guests_;
    std::vector<std::shared_ptr<Reservation>> reservations_;
    int nextReservationId_;
};

class OccupancyReport {
public:
    OccupancyReport(std::shared_ptr<Hotel> hotel, Date date);
    int bookedRooms() const;
    int occupancyPercent() const;
    Money expectedRevenue() const;

private:
    std::shared_ptr<Hotel> hotel_;
    Date date_;
};

}  // namespace hotel
