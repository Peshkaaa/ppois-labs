#include "fixtures.hpp"
#include "test_framework.hpp"

using namespace fixtures;

TEST_CASE(money_arithmetic_and_format) {
    CHECK(Money(150).add(Money(50)).getCents() == 200);
    CHECK(Money(150).subtract(Money(50)).getCents() == 100);
    CHECK(Money(150).multiply(2).getCents() == 300);
    CHECK(Money(1000).percentOf(10).getCents() == 100);
    CHECK(Money(0).isZero());
    CHECK(Money(-1).isNegative());
    CHECK(Money(1234).format() == "12.34");
    CHECK(Money(1205).format() == "12.05");
    CHECK(Money(-250).format() == "-2.50");
    CHECK(Money(1) < Money(2));
}

TEST_CASE(date_operations) {
    const Date day(5);
    CHECK(day.plusDays(3).getDayNumber() == 8);
    CHECK(day.daysTo(Date(9)) == 4);
    CHECK(day.isBefore(Date(6)));
    CHECK(day.isAfter(Date(4)));
    CHECK(day.label() == "Day 5");
    CHECK_THROWS_AS(Date(-1), InvalidDataException);
}

TEST_CASE(date_range_rules) {
    const DateRange stay = range(10, 13);
    CHECK(stay.nights() == 3);
    CHECK(stay.contains(Date(10)));
    CHECK(!stay.contains(Date(13)));
    CHECK(stay.overlaps(range(12, 15)));
    CHECK(!stay.overlaps(range(13, 15)));
    CHECK_THROWS_AS(DateRange(Date(5), Date(5)), InvalidDateRangeException);
}

TEST_CASE(address_validation_and_format) {
    const Address address("Main 1", "Minsk", "BY", "220000");
    CHECK(address.formatted() == "Main 1, 220000 Minsk, BY");
    CHECK(address.sameCity(Address("Other", "Minsk", "BY", "1")));
    CHECK(!address.sameCity(Address("Other", "Brest", "BY", "1")));
    CHECK(address.getCity() == "Minsk");
    CHECK_THROWS_AS(Address("", "c", "k", "1"), InvalidDataException);
    CHECK_THROWS_AS(Address("s", "", "k", "1"), InvalidDataException);
}

TEST_CASE(contact_info_rules) {
    const ContactInfo info = contact();
    CHECK(info.emailDomain() == "mail.test");
    CHECK(info.maskedPhone() == "**********567");
    CHECK(info.sameEmailDomain(ContactInfo("+375291111111", "a@mail.test")));
    CHECK_THROWS_AS(ContactInfo("+375291234567", "broken"), InvalidDataException);
    CHECK_THROWS_AS(ContactInfo("123", "a@b.c"), InvalidDataException);
}

TEST_CASE(validation_helpers) {
    CHECK_THROWS_AS(requireText("", "x"), InvalidDataException);
    CHECK_THROWS_AS(requirePositive(0, "x"), InvalidDataException);
    CHECK_THROWS_AS(requireNonNegative(-1, "x"), InvalidDataException);
    CHECK_THROWS_AS(requireNotNull(std::shared_ptr<int>(), "x"), InvalidDataException);
}

TEST_CASE(exceptions_share_common_base) {
    CHECK_THROWS_AS(throw RoomNotAvailableException(5), HotelException);
    CHECK_THROWS_AS(throw RoomNotAvailableException(std::string("x")), HotelException);
    CHECK_THROWS_AS(throw RoomNotFoundException(1), HotelException);
    CHECK_THROWS_AS(throw ReservationNotFoundException(1), HotelException);
    CHECK_THROWS_AS(throw GuestNotFoundException(1), HotelException);
    CHECK_THROWS_AS(throw InsufficientFundsException("x"), HotelException);
    CHECK_THROWS_AS(throw InvalidPaymentException("x"), HotelException);
    CHECK_THROWS_AS(throw CapacityExceededException(3, 2), HotelException);
    CHECK_THROWS_AS(throw InvalidStatusTransitionException("a", "b"), HotelException);
    CHECK_THROWS_AS(throw CancellationNotAllowedException(1), HotelException);
    CHECK_THROWS_AS(throw InvalidPromoCodeException("x"), HotelException);
    CHECK_THROWS_AS(throw EmployeeNotAvailableException("x"), HotelException);
    CHECK_THROWS_AS(throw ExpiredDocumentException("x"), HotelException);
}
