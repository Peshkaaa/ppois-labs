#pragma once

#include <stdexcept>
#include <string>

namespace hotel {

class HotelException : public std::runtime_error {
public:
    explicit HotelException(const std::string& message) : std::runtime_error(message) {}
};

class InvalidDataException : public HotelException {
public:
    explicit InvalidDataException(const std::string& detail) : HotelException("Invalid data: " + detail) {}
};

class InvalidDateRangeException : public HotelException {
public:
    InvalidDateRangeException(int startDay, int endDay)
        : HotelException("Invalid date range: " + std::to_string(startDay) + " -> " + std::to_string(endDay)) {}
};

class RoomNotAvailableException : public HotelException {
public:
    explicit RoomNotAvailableException(int roomNumber)
        : HotelException("Room " + std::to_string(roomNumber) + " is not available") {}
    explicit RoomNotAvailableException(const std::string& detail) : HotelException("Room unavailable: " + detail) {}
};

class RoomNotFoundException : public HotelException {
public:
    explicit RoomNotFoundException(int roomNumber)
        : HotelException("Room " + std::to_string(roomNumber) + " not found") {}
};

class ReservationNotFoundException : public HotelException {
public:
    explicit ReservationNotFoundException(int reservationId)
        : HotelException("Reservation " + std::to_string(reservationId) + " not found") {}
};

class GuestNotFoundException : public HotelException {
public:
    explicit GuestNotFoundException(int guestId) : HotelException("Guest " + std::to_string(guestId) + " not found") {}
};

class InsufficientFundsException : public HotelException {
public:
    explicit InsufficientFundsException(const std::string& resource) : HotelException("Insufficient " + resource) {}
};

class InvalidPaymentException : public HotelException {
public:
    explicit InvalidPaymentException(const std::string& detail) : HotelException("Invalid payment: " + detail) {}
};

class CapacityExceededException : public HotelException {
public:
    CapacityExceededException(int requested, int available)
        : HotelException("Requested " + std::to_string(requested) + " but only " + std::to_string(available) +
                         " available") {}
};

class InvalidStatusTransitionException : public HotelException {
public:
    InvalidStatusTransitionException(const std::string& from, const std::string& to)
        : HotelException("Invalid status transition: " + from + " -> " + to) {}
};

class CancellationNotAllowedException : public HotelException {
public:
    explicit CancellationNotAllowedException(int reservationId)
        : HotelException("Reservation " + std::to_string(reservationId) + " cannot be cancelled") {}
};

class InvalidPromoCodeException : public HotelException {
public:
    explicit InvalidPromoCodeException(const std::string& code) : HotelException("Promo code rejected: " + code) {}
};

class EmployeeNotAvailableException : public HotelException {
public:
    explicit EmployeeNotAvailableException(const std::string& detail)
        : HotelException("Employee not available: " + detail) {}
};

class ExpiredDocumentException : public HotelException {
public:
    explicit ExpiredDocumentException(const std::string& document) : HotelException("Document expired: " + document) {}
};

}  // namespace hotel
