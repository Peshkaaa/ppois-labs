#include "fixtures.hpp"
#include "test_framework.hpp"

using namespace fixtures;

TEST_CASE(cancellation_policy_penalty) {
    const CancellationPolicy policy(3, 50);
    CHECK(policy.isFreeCancel(Date(10), Date(5)));
    CHECK(!policy.isFreeCancel(Date(10), Date(9)));
    CHECK(policy.penaltyFor(Money(1000), Date(10), Date(5)).isZero());
    CHECK(policy.penaltyFor(Money(1000), Date(10), Date(9)).getCents() == 500);
    CHECK(policy.getFreeCancelDays() == 3 && policy.getPenaltyPercent() == 50);
    CHECK_THROWS_AS(CancellationPolicy(-1, 10), InvalidDataException);
    CHECK_THROWS_AS(CancellationPolicy(1, 101), InvalidDataException);
}

TEST_CASE(promo_code_redeem) {
    PromoCode promo("SUMMER", 10, range(0, 30), 1);
    CHECK(promo.isValidOn(Date(5)) && !promo.isValidOn(Date(30)));
    CHECK(promo.discountFor(Money(1000)).getCents() == 100);
    CHECK(promo.redeem(Date(5), Money(1000)).getCents() == 100);
    CHECK(promo.getUsesLeft() == 0);
    CHECK_THROWS_AS(promo.redeem(Date(5), Money(1000)), InvalidPromoCodeException);
    CHECK_THROWS_AS(PromoCode("", 10, range(), 1), InvalidDataException);
    CHECK(promo.getCode() == "SUMMER");
}

TEST_CASE(seasonal_rate) {
    const SeasonalRate high("High", range(10, 20), 150);
    CHECK(high.appliesOn(Date(10)) && !high.appliesOn(Date(20)));
    CHECK(high.adjust(Money(1000)).getCents() == 1500);
    CHECK(high.applyIfActive(Money(1000), Date(12)).getCents() == 1500);
    CHECK(high.applyIfActive(Money(1000), Date(30)).getCents() == 1000);
    CHECK_THROWS_AS(SeasonalRate("", range(), 100), InvalidDataException);
    CHECK(high.getName() == "High");
}

TEST_CASE(reservation_lifecycle) {
    auto r = reservation();
    CHECK(r->statusName() == "Pending" && r->isActive());
    CHECK(r->totalCost().getCents() == 21000);
    CHECK(r->overlapsWith(range(11, 13)));
    r->confirm();
    CHECK(r->statusName() == "Confirmed");
    CHECK_THROWS_AS(r->confirm(), InvalidStatusTransitionException);
    r->checkIn();
    CHECK(r->statusName() == "CheckedIn");
    CHECK_THROWS_AS(r->cancel(Date(1), CancellationPolicy(1, 10)), CancellationNotAllowedException);
    r->checkOut();
    CHECK(r->statusName() == "CheckedOut" && !r->isActive());
    CHECK(r->getId() == 1 && r->getGuestCount() == 2 && r->getStatus() == ReservationStatus::CheckedOut);
}

TEST_CASE(reservation_cancel_and_validation) {
    auto r = reservation();
    const Money penalty = r->cancel(Date(9), CancellationPolicy(3, 50));
    CHECK(penalty.getCents() == 10500 && r->statusName() == "Cancelled");
    CHECK_THROWS_AS(r->cancel(Date(9), CancellationPolicy(3, 50)), CancellationNotAllowedException);
    CHECK_THROWS_AS(Reservation(1, guest(), room(), range(), 5), CapacityExceededException);
    CHECK_THROWS_AS(Reservation(1, nullptr, room(), range(), 1), InvalidDataException);
    CHECK_THROWS_AS(Reservation(1, guest(), nullptr, range(), 1), InvalidDataException);
    CHECK(r->getGuest() != nullptr && r->getRoom() != nullptr && r->getStay().nights() == 2);
}

TEST_CASE(check_in_record) {
    auto r = reservation();
    r->confirm();
    CheckInRecord record(r, receptionist(), Date(10));
    const KeyCard card = record.performCheckIn("KC-1");
    CHECK(card.isValidOn(Date(10)) && r->getRoom()->statusName() == "Occupied" && record.getDate().getDayNumber() == 10);
    auto offDuty = reservation(2);
    offDuty->confirm();
    CHECK_THROWS_AS(CheckInRecord(offDuty, receptionist(false), Date(10)).performCheckIn("K"), EmployeeNotAvailableException);
    CHECK_THROWS_AS(CheckInRecord(nullptr, receptionist(), Date(1)), InvalidDataException);
    CHECK_THROWS_AS(CheckInRecord(r, nullptr, Date(1)), InvalidDataException);
}

TEST_CASE(check_in_rejects_expired_passport_and_busy_room) {
    auto expired = std::make_shared<Guest>(person(), 5, PassportData("AB1234567", "BY", Date(5)),
                                           std::make_shared<LoyaltyCard>("L"));
    auto r = std::make_shared<Reservation>(3, expired, room(), range(), 2);
    r->confirm();
    CHECK_THROWS_AS(CheckInRecord(r, receptionist(), Date(10)).performCheckIn("K"), ExpiredDocumentException);
    auto busy = reservation(4);
    busy->confirm();
    busy->getRoom()->occupy();
    CHECK_THROWS_AS(CheckInRecord(busy, receptionist(), Date(10)).performCheckIn("K"), RoomNotAvailableException);
}

TEST_CASE(check_out_record) {
    auto r = reservation();
    r->confirm();
    CheckInRecord(r, receptionist(), Date(10)).performCheckIn("K");
    CheckOutRecord record(r, Date(13));
    record.addExtraCharge(Money(1000));
    CHECK(record.getExtraCharges().getCents() == 1000 && record.wasLate());
    CHECK(record.finalAmount().getCents() == 22000);
    CHECK(record.performCheckOut().getCents() == 22000);
    CHECK(r->getRoom()->statusName() == "Cleaning");
    CHECK_THROWS_AS(record.addExtraCharge(Money(-1)), InvalidDataException);
    CHECK_THROWS_AS(CheckOutRecord(nullptr, Date(1)), InvalidDataException);
    CHECK(!CheckOutRecord(r, Date(12)).wasLate());
}

TEST_CASE(tax_rule_and_invoice_item) {
    const TaxRule tax("VAT", 20);
    CHECK(tax.taxFor(Money(1000)).getCents() == 200 && tax.withTax(Money(1000)).getCents() == 1200);
    CHECK(tax.getName() == "VAT" && tax.getRatePercent() == 20);
    InvoiceItem item("Bar", Money(300), 2);
    CHECK(item.total().getCents() == 600);
    item.increaseQuantity(1);
    CHECK(item.total().getCents() == 900 && item.getQuantity() == 3 && item.getDescription() == "Bar");
    CHECK_THROWS_AS(item.increaseQuantity(0), InvalidDataException);
    CHECK_THROWS_AS(InvoiceItem("", Money(1), 1), InvalidDataException);
    CHECK_THROWS_AS(TaxRule("", 1), InvalidDataException);
}

TEST_CASE(payment_processing) {
    Payment payment(1, Money(500), PaymentMethod::Card);
    CHECK(!payment.isPaid() && payment.methodName() == "Card");
    payment.process();
    CHECK(payment.isPaid() && payment.getPaymentId() == 1);
    CHECK_THROWS_AS(payment.process(), InvalidPaymentException);
    CHECK_THROWS_AS(Payment(2, Money(0), PaymentMethod::Cash), InvalidPaymentException);
    CHECK(Payment(3, Money(1), PaymentMethod::Cash).methodName() == "Cash");
    CHECK(Payment(4, Money(1), PaymentMethod::Transfer).methodName() == "Transfer");
}

TEST_CASE(invoice_totals_and_payments) {
    Invoice invoice(1, guest(), TaxRule("VAT", 10));
    invoice.addItem(InvoiceItem("Room", Money(1000), 1));
    CHECK(invoice.subtotal().getCents() == 1000 && invoice.taxAmount().getCents() == 100);
    CHECK(invoice.total().getCents() == 1100 && invoice.balanceDue().getCents() == 1100);
    invoice.addPayment(Payment(1, Money(600), PaymentMethod::Cash));
    CHECK(invoice.paidAmount().getCents() == 600 && !invoice.isSettled());
    CHECK_THROWS_AS(invoice.addPayment(Payment(2, Money(9999), PaymentMethod::Cash)), InvalidPaymentException);
    invoice.addPayment(Payment(3, Money(500), PaymentMethod::Card));
    CHECK(invoice.isSettled() && invoice.getInvoiceId() == 1);
    CHECK_THROWS_AS(Invoice(1, nullptr, TaxRule("V", 1)), InvalidDataException);
}

TEST_CASE(deposit_flow) {
    Invoice invoice(1, guest(), TaxRule("VAT", 0));
    invoice.addItem(InvoiceItem("Room", Money(1000), 1));
    Deposit deposit(reservation(), Money(400));
    CHECK(deposit.isHeld() && deposit.getAmount().getCents() == 400);
    deposit.applyToInvoice(invoice, 9);
    CHECK(!deposit.isHeld() && invoice.paidAmount().getCents() == 400);
    CHECK_THROWS_AS(deposit.applyToInvoice(invoice, 10), InvalidPaymentException);
    Deposit other(reservation(), Money(100));
    CHECK(other.release().getCents() == 100);
    CHECK_THROWS_AS(other.release(), InvalidPaymentException);
    CHECK_THROWS_AS(Deposit(nullptr, Money(1)), InvalidDataException);
}

TEST_CASE(refund_rules) {
    auto payment = std::make_shared<Payment>(1, Money(1000), PaymentMethod::Card);
    CHECK_THROWS_AS(Refund(payment, Money(100), "x"), InvalidPaymentException);
    payment->process();
    const Refund partial(payment, Money(400), "late arrival");
    CHECK(!partial.isFull() && partial.remainder().getCents() == 600 && partial.getAmount().getCents() == 400);
    CHECK(Refund(payment, Money(1000), "cancel").isFull());
    CHECK_THROWS_AS(Refund(payment, Money(2000), "x"), InvalidPaymentException);
    CHECK_THROWS_AS(Refund(nullptr, Money(1), "x"), InvalidDataException);
}
