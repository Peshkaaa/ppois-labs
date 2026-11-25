#include "hotel/booking.hpp"

#include "hotel/exceptions.hpp"
#include "hotel/validation.hpp"

namespace hotel {

CancellationPolicy::CancellationPolicy(int freeCancelDays, int penaltyPercent)
    : freeCancelDays_(freeCancelDays), penaltyPercent_(penaltyPercent) {
    requireNonNegative(freeCancelDays_, "free cancel days");
    if (penaltyPercent_ < 0 || penaltyPercent_ > constants::kPercentBase) {
        throw InvalidDataException("penalty percent out of range");
    }
}

bool CancellationPolicy::isFreeCancel(const Date& arrival, const Date& cancelDate) const {
    return cancelDate.daysTo(arrival) >= freeCancelDays_;
}

Money CancellationPolicy::penaltyFor(const Money& total, const Date& arrival, const Date& cancelDate) const {
    if (isFreeCancel(arrival, cancelDate)) {
        return Money(0);
    }
    return total.percentOf(penaltyPercent_);
}

PromoCode::PromoCode(std::string code, int discountPercent, DateRange validity, int usesLeft)
    : code_(std::move(code)), discountPercent_(discountPercent), validity_(validity), usesLeft_(usesLeft) {
    requireText(code_, "promo code");
    requirePositive(discountPercent_, "discount percent");
    requireNonNegative(usesLeft_, "uses left");
}

bool PromoCode::isValidOn(const Date& day) const { return usesLeft_ > 0 && validity_.contains(day); }

Money PromoCode::discountFor(const Money& total) const { return total.percentOf(discountPercent_); }

Money PromoCode::redeem(const Date& day, const Money& total) {
    if (!isValidOn(day)) {
        throw InvalidPromoCodeException(code_);
    }
    --usesLeft_;
    return discountFor(total);
}

SeasonalRate::SeasonalRate(std::string name, DateRange period, int multiplierPercent)
    : name_(std::move(name)), period_(period), multiplierPercent_(multiplierPercent) {
    requireText(name_, "season name");
    requirePositive(multiplierPercent_, "multiplier percent");
}

bool SeasonalRate::appliesOn(const Date& day) const { return period_.contains(day); }

Money SeasonalRate::adjust(const Money& base) const { return base.percentOf(multiplierPercent_); }

Money SeasonalRate::applyIfActive(const Money& base, const Date& day) const {
    return appliesOn(day) ? adjust(base) : base;
}

Reservation::Reservation(int id, std::shared_ptr<Guest> guest, std::shared_ptr<Room> room, DateRange stay,
                         int guestCount)
    : id_(id), guest_(std::move(guest)), room_(std::move(room)), stay_(stay), status_(ReservationStatus::Pending),
      guestCount_(guestCount) {
    requireNotNull(guest_, "guest");
    requireNotNull(room_, "room");
    if (!room_->getType()->canHost(guestCount_)) {
        throw CapacityExceededException(guestCount_, room_->getType()->getCapacity());
    }
}

void Reservation::moveTo(ReservationStatus expected, ReservationStatus target) {
    if (status_ != expected) {
        throw InvalidStatusTransitionException(statusName(), "next status");
    }
    status_ = target;
}

void Reservation::confirm() { moveTo(ReservationStatus::Pending, ReservationStatus::Confirmed); }

void Reservation::checkIn() { moveTo(ReservationStatus::Confirmed, ReservationStatus::CheckedIn); }

void Reservation::checkOut() { moveTo(ReservationStatus::CheckedIn, ReservationStatus::CheckedOut); }

Money Reservation::cancel(const Date& today, const CancellationPolicy& policy) {
    if (status_ == ReservationStatus::CheckedIn || !isActive()) {
        throw CancellationNotAllowedException(id_);
    }
    status_ = ReservationStatus::Cancelled;
    return policy.penaltyFor(totalCost(), stay_.getStart(), today);
}

Money Reservation::totalCost() const { return room_->ratePerNight().multiply(stay_.nights()); }

bool Reservation::isActive() const {
    return status_ != ReservationStatus::Cancelled && status_ != ReservationStatus::CheckedOut;
}

bool Reservation::overlapsWith(const DateRange& range) const { return stay_.overlaps(range); }

std::string Reservation::statusName() const {
    switch (status_) {
        case ReservationStatus::Confirmed:
            return "Confirmed";
        case ReservationStatus::CheckedIn:
            return "CheckedIn";
        case ReservationStatus::CheckedOut:
            return "CheckedOut";
        case ReservationStatus::Cancelled:
            return "Cancelled";
        default:
            return "Pending";
    }
}

CheckInRecord::CheckInRecord(std::shared_ptr<Reservation> reservation, std::shared_ptr<Receptionist> receptionist,
                             Date date)
    : reservation_(std::move(reservation)), receptionist_(std::move(receptionist)), date_(date) {
    requireNotNull(reservation_, "reservation");
    requireNotNull(receptionist_, "receptionist");
}

void CheckInRecord::verifyDocuments() const {
    reservation_->getGuest()->getPassport().ensureValidOn(reservation_->getStay().getEnd());
}

KeyCard CheckInRecord::performCheckIn(const std::string& cardId) {
    if (!receptionist_->isOnDuty()) {
        throw EmployeeNotAvailableException(receptionist_->fullName() + " is not on duty");
    }
    verifyDocuments();
    const auto& room = reservation_->getRoom();
    if (!room->isAvailable()) {
        throw RoomNotAvailableException(room->getNumber());
    }
    reservation_->checkIn();
    room->occupy();
    return KeyCard(cardId, room, reservation_->getStay());
}

CheckOutRecord::CheckOutRecord(std::shared_ptr<Reservation> reservation, Date date)
    : reservation_(std::move(reservation)), date_(date), extraCharges_(Money(0)) {
    requireNotNull(reservation_, "reservation");
}

void CheckOutRecord::addExtraCharge(const Money& charge) {
    requireNonNegative(charge.getCents(), "extra charge");
    extraCharges_ = extraCharges_.add(charge);
}

Money CheckOutRecord::finalAmount() const { return reservation_->totalCost().add(extraCharges_); }

bool CheckOutRecord::wasLate() const { return date_.isAfter(reservation_->getStay().getEnd()); }

Money CheckOutRecord::performCheckOut() {
    reservation_->checkOut();
    reservation_->getRoom()->release();
    return finalAmount();
}

}  // namespace hotel
