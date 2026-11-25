#include "hotel/facilities.hpp"

#include <algorithm>

#include "hotel/exceptions.hpp"
#include "hotel/validation.hpp"

namespace hotel {

ParkingSpot::ParkingSpot(int number) : number_(number), occupied_(false) { requirePositive(number_, "spot number"); }

void ParkingSpot::park(const std::string& plate) {
    requireText(plate, "plate");
    if (occupied_) {
        throw InvalidStatusTransitionException("Occupied", "Occupied");
    }
    occupied_ = true;
    plate_ = plate;
}

void ParkingSpot::leave() {
    if (!occupied_) {
        throw InvalidStatusTransitionException("Free", "Free");
    }
    occupied_ = false;
    plate_.clear();
}

Parking::Parking(Money dailyFee) : dailyFee_(dailyFee) { requireNonNegative(dailyFee_.getCents(), "daily fee"); }

void Parking::addSpot(const ParkingSpot& spot) { spots_.push_back(spot); }

int Parking::freeSpots() const {
    return static_cast<int>(std::count_if(spots_.begin(), spots_.end(), [](const ParkingSpot& spot) { return spot.isFree(); }));
}

int Parking::parkCar(const std::string& plate) {
    for (ParkingSpot& spot : spots_) {
        if (spot.isFree()) {
            spot.park(plate);
            return spot.getNumber();
        }
    }
    throw CapacityExceededException(1, 0);
}

Money Parking::feeForDays(int days) const { return dailyFee_.multiply(days); }

ConferenceHall::ConferenceHall(std::string name, int capacity, Money hourlyRate)
    : name_(std::move(name)), capacity_(capacity), hourlyRate_(hourlyRate) {
    requireText(name_, "hall name");
    requirePositive(capacity_, "hall capacity");
    requireNonNegative(hourlyRate_.getCents(), "hourly rate");
}

bool ConferenceHall::fits(int attendees) const { return attendees > 0 && attendees <= capacity_; }

Money ConferenceHall::costForHours(int hours) const { return hourlyRate_.multiply(hours); }

ConferenceBooking::ConferenceBooking(std::shared_ptr<ConferenceHall> hall, std::shared_ptr<Guest> organizer,
                                     DateRange period, int attendees)
    : hall_(std::move(hall)), organizer_(std::move(organizer)), period_(period), attendees_(attendees) {
    requireNotNull(hall_, "hall");
    requireNotNull(organizer_, "organizer");
    if (!hall_->fits(attendees_)) {
        throw CapacityExceededException(attendees_, hall_->getCapacity());
    }
}

Money ConferenceBooking::totalCost(int hoursPerDay) const {
    return hall_->costForHours(hoursPerDay * period_.nights());
}

bool ConferenceBooking::conflictsWith(const ConferenceBooking& other) const {
    return hall_ == other.hall_ && period_.overlaps(other.period_);
}

SpaTreatment::SpaTreatment(std::string name, int durationMinutes, Money price)
    : name_(std::move(name)), durationMinutes_(durationMinutes), price_(price) {
    requireText(name_, "treatment name");
    requirePositive(durationMinutes_, "duration");
    requireNonNegative(price_.getCents(), "price");
}

Money SpaTreatment::priceForGuest(const Guest& guest) const {
    return price_.subtract(price_.percentOf(guest.getLoyaltyCard()->discountPercent()));
}

int SpaTreatment::endMinute(int startMinute) const { return startMinute + durationMinutes_; }

SpaBooking::SpaBooking(std::shared_ptr<SpaTreatment> treatment, std::shared_ptr<Guest> guest, int startMinute)
    : treatment_(std::move(treatment)), guest_(std::move(guest)), startMinute_(startMinute) {
    requireNotNull(treatment_, "treatment");
    requireNotNull(guest_, "guest");
    requireNonNegative(startMinute_, "start minute");
}

int SpaBooking::endMinute() const { return treatment_->endMinute(startMinute_); }

bool SpaBooking::overlaps(const SpaBooking& other) const {
    return startMinute_ < other.endMinute() && other.startMinute_ < endMinute();
}

Money SpaBooking::cost() const { return treatment_->priceForGuest(*guest_); }

ShuttleTransfer::ShuttleTransfer(std::shared_ptr<Guest> guest, std::string destination, Money price)
    : guest_(std::move(guest)), destination_(std::move(destination)), price_(price), confirmed_(false) {
    requireNotNull(guest_, "guest");
    requireText(destination_, "destination");
}

void ShuttleTransfer::confirm() {
    if (confirmed_) {
        throw InvalidStatusTransitionException("Confirmed", "Confirmed");
    }
    confirmed_ = true;
}

void ShuttleTransfer::cancel() {
    if (!confirmed_) {
        throw InvalidStatusTransitionException("Unconfirmed", "Cancelled");
    }
    confirmed_ = false;
}

InvoiceItem ShuttleTransfer::toInvoiceItem() const {
    if (!confirmed_) {
        throw InvalidStatusTransitionException("Unconfirmed", "Invoiced");
    }
    return InvoiceItem("Shuttle to " + destination_, price_, 1);
}

}  // namespace hotel
