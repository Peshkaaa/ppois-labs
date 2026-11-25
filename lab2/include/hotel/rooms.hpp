#pragma once

#include <memory>
#include <string>
#include <vector>

#include "hotel/common.hpp"

namespace hotel {

class Amenity {
public:
    Amenity(std::string name, Money dailyCharge);
    const std::string& getName() const { return name_; }
    const Money& getDailyCharge() const { return dailyCharge_; }
    bool isComplimentary() const;
    Money chargeForNights(int nights) const;
    std::string describe() const;

private:
    std::string name_;
    Money dailyCharge_;
};

enum class BedKind { Single, Double, King };

class Bed {
public:
    Bed(BedKind kind, int capacity);
    BedKind getKind() const { return kind_; }
    int getCapacity() const { return capacity_; }
    bool fitsGuests(int guests) const;
    std::string kindName() const;

private:
    BedKind kind_;
    int capacity_;
};

class RoomType {
public:
    RoomType(std::string name, int capacity, Money baseRate);
    const std::string& getName() const { return name_; }
    int getCapacity() const { return capacity_; }
    const Money& getBaseRate() const { return baseRate_; }
    void addBed(const Bed& bed);
    void addAmenity(const Amenity& amenity);
    int totalBedCapacity() const;
    Money amenitiesDailyCost() const;
    bool hasAmenity(const std::string& name) const;
    bool canHost(int guests) const;
    Money nightlyRate() const;

private:
    std::string name_;
    int capacity_;
    Money baseRate_;
    std::vector<Bed> beds_;
    std::vector<Amenity> amenities_;
};

enum class RoomStatus { Available, Occupied, Cleaning, Maintenance };

class Room {
public:
    Room(int number, int floor, std::shared_ptr<RoomType> type);
    int getNumber() const { return number_; }
    int getFloor() const { return floor_; }
    const std::shared_ptr<RoomType>& getType() const { return type_; }
    RoomStatus getStatus() const { return status_; }
    bool isAvailable() const;
    void occupy();
    void release();
    void markAvailable();
    void sendToMaintenance();
    Money ratePerNight() const;
    std::string statusName() const;

private:
    int number_;
    int floor_;
    std::shared_ptr<RoomType> type_;
    RoomStatus status_;
};

class Floor {
public:
    explicit Floor(int number);
    int getNumber() const { return number_; }
    const std::vector<std::shared_ptr<Room>>& getRooms() const { return rooms_; }
    void addRoom(const std::shared_ptr<Room>& room);
    std::shared_ptr<Room> findRoom(int roomNumber) const;
    std::vector<std::shared_ptr<Room>> availableRooms() const;
    int occupiedCount() const;

private:
    int number_;
    std::vector<std::shared_ptr<Room>> rooms_;
};

class KeyCard {
public:
    KeyCard(std::string cardId, std::shared_ptr<Room> room, DateRange validity);
    const std::string& getCardId() const { return cardId_; }
    const DateRange& getValidity() const { return validity_; }
    bool isActive() const { return active_; }
    bool isValidOn(const Date& day) const;
    bool opens(const Room& room, const Date& day) const;
    void deactivate();
    void extend(const Date& newEnd);

private:
    std::string cardId_;
    std::shared_ptr<Room> room_;
    DateRange validity_;
    bool active_;
};

}  // namespace hotel
