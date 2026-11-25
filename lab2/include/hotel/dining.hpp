#pragma once

#include <memory>
#include <string>
#include <vector>

#include "hotel/billing.hpp"
#include "hotel/housekeeping.hpp"

namespace hotel {

class MenuItem {
public:
    MenuItem(std::string name, Money price, std::string category);
    const std::string& getName() const { return name_; }
    const Money& getPrice() const { return price_; }
    bool isAvailable() const { return available_; }
    void markUnavailable();
    void markAvailable();
    Money priceWithTax(const TaxRule& tax) const;

private:
    std::string name_;
    Money price_;
    std::string category_;
    bool available_;
};

class Menu {
public:
    explicit Menu(std::string title);
    const std::string& getTitle() const { return title_; }
    void addItem(const std::shared_ptr<MenuItem>& item);
    std::shared_ptr<MenuItem> findItem(const std::string& name) const;
    std::vector<std::shared_ptr<MenuItem>> availableItems() const;
    Money averagePrice() const;

private:
    std::string title_;
    std::vector<std::shared_ptr<MenuItem>> items_;
};

class Table {
public:
    Table(int number, int seats);
    int getNumber() const { return number_; }
    int getSeats() const { return seats_; }
    bool isReserved() const { return reserved_; }
    bool fits(int guests) const;
    void reserve(int guests);
    void release();

private:
    int number_;
    int seats_;
    bool reserved_;
};

class Restaurant {
public:
    Restaurant(std::string name, std::shared_ptr<Menu> menu);
    const std::string& getName() const { return name_; }
    void addTable(const Table& table);
    int seatParty(int guests);
    int freeTableCount() const;
    int largestTable() const;

private:
    std::string name_;
    std::shared_ptr<Menu> menu_;
    std::vector<Table> tables_;
};

class Waiter : public Employee {
public:
    explicit Waiter(Employee base);
    const Money& getTips() const { return tips_; }
    void assignTable(int tableNumber);
    void addTip(const Money& tip);
    bool servesTable(int tableNumber) const;

private:
    std::vector<int> assignedTables_;
    Money tips_;
};

class RoomServiceOrder {
public:
    explicit RoomServiceOrder(std::shared_ptr<Room> room);
    bool isDelivered() const { return delivered_; }
    void addItem(const std::shared_ptr<MenuItem>& item);
    Money total() const;
    void assignWaiter(const std::shared_ptr<Waiter>& waiter);
    void deliver();
    InvoiceItem toInvoiceItem() const;

private:
    std::shared_ptr<Room> room_;
    std::vector<std::shared_ptr<MenuItem>> items_;
    std::shared_ptr<Waiter> waiter_;
    bool delivered_;
};

}  // namespace hotel
