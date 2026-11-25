#pragma once

#include <memory>
#include <string>
#include <vector>

#include "hotel/booking.hpp"

namespace hotel {

class TaxRule {
public:
    TaxRule(std::string name, int ratePercent);
    const std::string& getName() const { return name_; }
    int getRatePercent() const { return ratePercent_; }
    Money taxFor(const Money& base) const;
    Money withTax(const Money& base) const;

private:
    std::string name_;
    int ratePercent_;
};

class InvoiceItem {
public:
    InvoiceItem(std::string description, Money unitPrice, int quantity);
    const std::string& getDescription() const { return description_; }
    int getQuantity() const { return quantity_; }
    Money total() const;
    void increaseQuantity(int extra);

private:
    std::string description_;
    Money unitPrice_;
    int quantity_;
};

enum class PaymentMethod { Cash, Card, Transfer };

class Payment {
public:
    Payment(int paymentId, Money amount, PaymentMethod method);
    int getPaymentId() const { return paymentId_; }
    const Money& getAmount() const { return amount_; }
    bool isPaid() const { return paid_; }
    void process();
    std::string methodName() const;

private:
    int paymentId_;
    Money amount_;
    PaymentMethod method_;
    bool paid_;
};

class Invoice {
public:
    Invoice(int invoiceId, std::shared_ptr<Guest> guest, TaxRule taxRule);
    int getInvoiceId() const { return invoiceId_; }
    void addItem(const InvoiceItem& item);
    Money subtotal() const;
    Money taxAmount() const;
    Money total() const;
    void addPayment(Payment payment);
    Money paidAmount() const;
    Money balanceDue() const;
    bool isSettled() const;

private:
    int invoiceId_;
    std::shared_ptr<Guest> guest_;
    std::vector<InvoiceItem> items_;
    std::vector<Payment> payments_;
    TaxRule taxRule_;
};

class Deposit {
public:
    Deposit(std::shared_ptr<Reservation> reservation, Money amount);
    const Money& getAmount() const { return amount_; }
    bool isHeld() const { return held_; }
    void applyToInvoice(Invoice& invoice, int paymentId);
    Money release();

private:
    std::shared_ptr<Reservation> reservation_;
    Money amount_;
    bool held_;
};

class Refund {
public:
    Refund(std::shared_ptr<Payment> payment, Money amount, std::string reason);
    const Money& getAmount() const { return amount_; }
    bool isFull() const;
    Money remainder() const;

private:
    std::shared_ptr<Payment> payment_;
    Money amount_;
    std::string reason_;
};

}  // namespace hotel
