#include "hotel/billing.hpp"

#include "hotel/exceptions.hpp"
#include "hotel/validation.hpp"

namespace hotel {

TaxRule::TaxRule(std::string name, int ratePercent) : name_(std::move(name)), ratePercent_(ratePercent) {
    requireText(name_, "tax name");
    requireNonNegative(ratePercent_, "tax rate");
}

Money TaxRule::taxFor(const Money& base) const { return base.percentOf(ratePercent_); }

Money TaxRule::withTax(const Money& base) const { return base.add(taxFor(base)); }

InvoiceItem::InvoiceItem(std::string description, Money unitPrice, int quantity)
    : description_(std::move(description)), unitPrice_(unitPrice), quantity_(quantity) {
    requireText(description_, "item description");
    requireNonNegative(unitPrice_.getCents(), "unit price");
    requirePositive(quantity_, "quantity");
}

Money InvoiceItem::total() const { return unitPrice_.multiply(quantity_); }

void InvoiceItem::increaseQuantity(int extra) {
    requirePositive(extra, "extra quantity");
    quantity_ += extra;
}

Payment::Payment(int paymentId, Money amount, PaymentMethod method)
    : paymentId_(paymentId), amount_(amount), method_(method), paid_(false) {
    if (amount_.isZero() || amount_.isNegative()) {
        throw InvalidPaymentException("amount must be positive");
    }
}

void Payment::process() {
    if (paid_) {
        throw InvalidPaymentException("payment already processed");
    }
    paid_ = true;
}

std::string Payment::methodName() const {
    switch (method_) {
        case PaymentMethod::Card:
            return "Card";
        case PaymentMethod::Transfer:
            return "Transfer";
        default:
            return "Cash";
    }
}

Invoice::Invoice(int invoiceId, std::shared_ptr<Guest> guest, TaxRule taxRule)
    : invoiceId_(invoiceId), guest_(std::move(guest)), taxRule_(std::move(taxRule)) {
    requireNotNull(guest_, "guest");
}

void Invoice::addItem(const InvoiceItem& item) { items_.push_back(item); }

Money Invoice::subtotal() const {
    Money sum;
    for (const InvoiceItem& item : items_) {
        sum = sum.add(item.total());
    }
    return sum;
}

Money Invoice::taxAmount() const { return taxRule_.taxFor(subtotal()); }

Money Invoice::total() const { return taxRule_.withTax(subtotal()); }

void Invoice::addPayment(Payment payment) {
    if (payment.getAmount() > balanceDue()) {
        throw InvalidPaymentException("overpayment");
    }
    payment.process();
    payments_.push_back(payment);
}

Money Invoice::paidAmount() const {
    Money sum;
    for (const Payment& payment : payments_) {
        sum = sum.add(payment.getAmount());
    }
    return sum;
}

Money Invoice::balanceDue() const { return total().subtract(paidAmount()); }

bool Invoice::isSettled() const { return balanceDue().isZero(); }

Deposit::Deposit(std::shared_ptr<Reservation> reservation, Money amount)
    : reservation_(std::move(reservation)), amount_(amount), held_(true) {
    requireNotNull(reservation_, "reservation");
    requireNonNegative(amount_.getCents(), "deposit");
}

void Deposit::applyToInvoice(Invoice& invoice, int paymentId) {
    if (!held_) {
        throw InvalidPaymentException("deposit is not held");
    }
    invoice.addPayment(Payment(paymentId, amount_, PaymentMethod::Card));
    held_ = false;
}

Money Deposit::release() {
    if (!held_) {
        throw InvalidPaymentException("deposit is not held");
    }
    held_ = false;
    return amount_;
}

Refund::Refund(std::shared_ptr<Payment> payment, Money amount, std::string reason)
    : payment_(std::move(payment)), amount_(amount), reason_(std::move(reason)) {
    requireNotNull(payment_, "payment");
    if (!payment_->isPaid() || amount_ > payment_->getAmount()) {
        throw InvalidPaymentException("refund is not possible");
    }
}

bool Refund::isFull() const { return amount_ == payment_->getAmount(); }

Money Refund::remainder() const { return payment_->getAmount().subtract(amount_); }

}  // namespace hotel
