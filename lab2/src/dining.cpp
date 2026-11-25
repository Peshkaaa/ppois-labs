#include <iterator>
#include "hotel/dining.hpp"

#include <algorithm>

#include "hotel/exceptions.hpp"
#include "hotel/validation.hpp"

namespace hotel {

MenuItem::MenuItem(std::string name, Money price, std::string category)
    : name_(std::move(name)), price_(price), category_(std::move(category)), available_(true) {
    requireText(name_, "menu item name");
    requireText(category_, "category");
    requireNonNegative(price_.getCents(), "price");
}

void MenuItem::markUnavailable() { available_ = false; }

void MenuItem::markAvailable() { available_ = true; }

Money MenuItem::priceWithTax(const TaxRule& tax) const { return tax.withTax(price_); }

Menu::Menu(std::string title) : title_(std::move(title)) { requireText(title_, "menu title"); }

void Menu::addItem(const std::shared_ptr<MenuItem>& item) {
    requireNotNull(item, "menu item");
    items_.push_back(item);
}

std::shared_ptr<MenuItem> Menu::findItem(const std::string& name) const {
    const auto found = std::find_if(items_.begin(), items_.end(),
                                    [&](const std::shared_ptr<MenuItem>& item) { return item->getName() == name; });
    if (found == items_.end()) {
        throw InvalidDataException("no such menu item: " + name);
    }
    return *found;
}

std::vector<std::shared_ptr<MenuItem>> Menu::availableItems() const {
    std::vector<std::shared_ptr<MenuItem>> result;
    std::copy_if(items_.begin(), items_.end(), std::back_inserter(result),
                 [](const std::shared_ptr<MenuItem>& item) { return item->isAvailable(); });
    return result;
}

Money Menu::averagePrice() const {
    if (items_.empty()) {
        return Money(0);
    }
    Money sum;
    for (const auto& item : items_) {
        sum = sum.add(item->getPrice());
    }
    return Money(sum.getCents() / static_cast<long long>(items_.size()));
}

Table::Table(int number, int seats) : number_(number), seats_(seats), reserved_(false) {
    requirePositive(number_, "table number");
    requirePositive(seats_, "seats");
}

bool Table::fits(int guests) const { return guests > 0 && guests <= seats_; }

void Table::reserve(int guests) {
    if (reserved_) {
        throw InvalidStatusTransitionException("Reserved", "Reserved");
    }
    if (!fits(guests)) {
        throw CapacityExceededException(guests, seats_);
    }
    reserved_ = true;
}

void Table::release() { reserved_ = false; }

Restaurant::Restaurant(std::string name, std::shared_ptr<Menu> menu) : name_(std::move(name)), menu_(std::move(menu)) {
    requireText(name_, "restaurant name");
    requireNotNull(menu_, "menu");
}

void Restaurant::addTable(const Table& table) { tables_.push_back(table); }

int Restaurant::seatParty(int guests) {
    for (Table& table : tables_) {
        if (!table.isReserved() && table.fits(guests)) {
            table.reserve(guests);
            return table.getNumber();
        }
    }
    throw CapacityExceededException(guests, largestTable());
}

int Restaurant::freeTableCount() const {
    return static_cast<int>(
        std::count_if(tables_.begin(), tables_.end(), [](const Table& table) { return !table.isReserved(); }));
}

int Restaurant::largestTable() const {
    int largest = 0;
    for (const Table& table : tables_) {
        largest = std::max(largest, table.getSeats());
    }
    return largest;
}

Waiter::Waiter(Employee base) : Employee(std::move(base)), tips_(Money(0)) {}

void Waiter::assignTable(int tableNumber) {
    requirePositive(tableNumber, "table number");
    if (!servesTable(tableNumber)) {
        assignedTables_.push_back(tableNumber);
    }
}

void Waiter::addTip(const Money& tip) {
    requireNonNegative(tip.getCents(), "tip");
    tips_ = tips_.add(tip);
}

bool Waiter::servesTable(int tableNumber) const {
    return std::find(assignedTables_.begin(), assignedTables_.end(), tableNumber) != assignedTables_.end();
}

RoomServiceOrder::RoomServiceOrder(std::shared_ptr<Room> room) : room_(std::move(room)), delivered_(false) {
    requireNotNull(room_, "room");
    if (room_->getStatus() != RoomStatus::Occupied) {
        throw InvalidStatusTransitionException(room_->statusName(), "Occupied");
    }
}

void RoomServiceOrder::addItem(const std::shared_ptr<MenuItem>& item) {
    requireNotNull(item, "menu item");
    if (!item->isAvailable()) {
        throw InvalidDataException(item->getName() + " is unavailable");
    }
    items_.push_back(item);
}

Money RoomServiceOrder::total() const {
    Money sum;
    for (const auto& item : items_) {
        sum = sum.add(item->getPrice());
    }
    return sum;
}

void RoomServiceOrder::assignWaiter(const std::shared_ptr<Waiter>& waiter) {
    requireNotNull(waiter, "waiter");
    waiter_ = waiter;
}

void RoomServiceOrder::deliver() {
    if (!waiter_) {
        throw EmployeeNotAvailableException("no waiter assigned");
    }
    if (delivered_) {
        throw InvalidStatusTransitionException("Delivered", "Delivered");
    }
    delivered_ = true;
}

InvoiceItem RoomServiceOrder::toInvoiceItem() const { return InvoiceItem("Room service", total(), 1); }

}  // namespace hotel
