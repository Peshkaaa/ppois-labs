#pragma once

#include <memory>
#include <string>

#include "hotel/people.hpp"
#include "hotel/rooms.hpp"

namespace hotel {

class CancellationPolicy {
public:
    CancellationPolicy(int freeCancelDays, int penaltyPercent);
    int getFreeCancelDays() const { return freeCancelDays_; }
    int getPenaltyPercent() const { return penaltyPercent_; }
    bool isFreeCancel(const Date& arrival, const Date& cancelDate) const;
    Money penaltyFor(const Money& total, const Date& arrival, const Date& cancelDate) const;

private:
    int freeCancelDays_;
    int penaltyPercent_;
};

class PromoCode {
public:
    PromoCode(std::string code, int discountPercent, DateRange validity, int usesLeft);
    const std::string& getCode() const { return code_; }
    int getUsesLeft() const { return usesLeft_; }
    bool isValidOn(const Date& day) const;
    Money discountFor(const Money& total) const;
    Money redeem(const Date& day, const Money& total);

private:
    std::string code_;
    int discountPercent_;
    DateRange validity_;
    int usesLeft_;
};

class SeasonalRate {
public:
    SeasonalRate(std::string name, DateRange period, int multiplierPercent);
    const std::string& getName() const { return name_; }
    bool appliesOn(const Date& day) const;
    Money adjust(const Money& base) const;
    Money applyIfActive(const Money& base, const Date& day) const;

private:
    std::string name_;
    DateRange period_;
    int multiplierPercent_;
};

enum class ReservationStatus { Pending, Confirmed, CheckedIn, CheckedOut, Cancelled };

class Reservation {
public:
    Reservation(int id, std::shared_ptr<Guest> guest, std::shared_ptr<Room> room, DateRange stay, int guestCount);
    int getId() const { return id_; }
    const std::shared_ptr<Guest>& getGuest() const { return guest_; }
    const std::shared_ptr<Room>& getRoom() const { return room_; }
    const DateRange& getStay() const { return stay_; }
    ReservationStatus getStatus() const { return status_; }
    int getGuestCount() const { return guestCount_; }
    void confirm();
    void checkIn();
    void checkOut();
    Money cancel(const Date& today, const CancellationPolicy& policy);
    Money totalCost() const;
    bool isActive() const;
    bool overlapsWith(const DateRange& range) const;
    std::string statusName() const;

private:
    void moveTo(ReservationStatus expected, ReservationStatus target);

    int id_;
    std::shared_ptr<Guest> guest_;
    std::shared_ptr<Room> room_;
    DateRange stay_;
    ReservationStatus status_;
    int guestCount_;
};

class CheckInRecord {
public:
    CheckInRecord(std::shared_ptr<Reservation> reservation, std::shared_ptr<Receptionist> receptionist, Date date);
    const Date& getDate() const { return date_; }
    void verifyDocuments() const;
    KeyCard performCheckIn(const std::string& cardId);

private:
    std::shared_ptr<Reservation> reservation_;
    std::shared_ptr<Receptionist> receptionist_;
    Date date_;
};

class CheckOutRecord {
public:
    CheckOutRecord(std::shared_ptr<Reservation> reservation, Date date);
    const Money& getExtraCharges() const { return extraCharges_; }
    void addExtraCharge(const Money& charge);
    Money finalAmount() const;
    bool wasLate() const;
    Money performCheckOut();

private:
    std::shared_ptr<Reservation> reservation_;
    Date date_;
    Money extraCharges_;
};

}  // namespace hotel
