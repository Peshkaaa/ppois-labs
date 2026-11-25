#pragma once

#include <memory>
#include <string>
#include <vector>

#include "hotel/billing.hpp"

namespace hotel {

class ParkingSpot {
public:
    explicit ParkingSpot(int number);
    int getNumber() const { return number_; }
    bool isFree() const { return !occupied_; }
    void park(const std::string& plate);
    void leave();

private:
    int number_;
    bool occupied_;
    std::string plate_;
};

class Parking {
public:
    explicit Parking(Money dailyFee);
    void addSpot(const ParkingSpot& spot);
    int freeSpots() const;
    int parkCar(const std::string& plate);
    Money feeForDays(int days) const;

private:
    std::vector<ParkingSpot> spots_;
    Money dailyFee_;
};

class ConferenceHall {
public:
    ConferenceHall(std::string name, int capacity, Money hourlyRate);
    const std::string& getName() const { return name_; }
    int getCapacity() const { return capacity_; }
    bool fits(int attendees) const;
    Money costForHours(int hours) const;

private:
    std::string name_;
    int capacity_;
    Money hourlyRate_;
};

class ConferenceBooking {
public:
    ConferenceBooking(std::shared_ptr<ConferenceHall> hall, std::shared_ptr<Guest> organizer, DateRange period,
                      int attendees);
    const DateRange& getPeriod() const { return period_; }
    Money totalCost(int hoursPerDay) const;
    bool conflictsWith(const ConferenceBooking& other) const;

private:
    std::shared_ptr<ConferenceHall> hall_;
    std::shared_ptr<Guest> organizer_;
    DateRange period_;
    int attendees_;
};

class SpaTreatment {
public:
    SpaTreatment(std::string name, int durationMinutes, Money price);
    const std::string& getName() const { return name_; }
    Money priceForGuest(const Guest& guest) const;
    int endMinute(int startMinute) const;

private:
    std::string name_;
    int durationMinutes_;
    Money price_;
};

class SpaBooking {
public:
    SpaBooking(std::shared_ptr<SpaTreatment> treatment, std::shared_ptr<Guest> guest, int startMinute);
    int getStartMinute() const { return startMinute_; }
    int endMinute() const;
    bool overlaps(const SpaBooking& other) const;
    Money cost() const;

private:
    std::shared_ptr<SpaTreatment> treatment_;
    std::shared_ptr<Guest> guest_;
    int startMinute_;
};

class ShuttleTransfer {
public:
    ShuttleTransfer(std::shared_ptr<Guest> guest, std::string destination, Money price);
    bool isConfirmed() const { return confirmed_; }
    void confirm();
    void cancel();
    InvoiceItem toInvoiceItem() const;

private:
    std::shared_ptr<Guest> guest_;
    std::string destination_;
    Money price_;
    bool confirmed_;
};

}  // namespace hotel
