#include "console_menu.hpp"

#include <istream>
#include <ostream>
#include <stdexcept>

#include "hotel/billing.hpp"
#include "hotel/exceptions.hpp"

namespace hotel::cli {

namespace {

constexpr int kExitChoice = 0;
constexpr int kListRooms = 1;
constexpr int kRegisterGuest = 2;
constexpr int kBook = 3;
constexpr int kConfirm = 4;
constexpr int kCheckIn = 5;
constexpr int kCheckOut = 6;
constexpr int kCancel = 7;
constexpr int kReport = 8;

struct InputEnded {};

}  // namespace

ConsoleMenu::ConsoleMenu(std::shared_ptr<Hotel> hotel, std::shared_ptr<Receptionist> receptionist,
                         std::istream& input, std::ostream& output)
    : hotel_(std::move(hotel)), receptionist_(std::move(receptionist)), input_(input), output_(output),
      nextGuestId_(1) {}

void ConsoleMenu::run() {
    try {
        bool running = true;
        while (running) {
            running = step();
        }
    } catch (const InputEnded&) {
        output_ << "\nInput ended.\n";
    }
}

bool ConsoleMenu::step() {
    printMenu();
    try {
        return execute(askInt("Choice"));
    } catch (const HotelException& error) {
        output_ << "Error: " << error.what() << "\n";
    } catch (const std::logic_error& error) {
        output_ << "Input error: " << error.what() << "\n";
    }
    return true;
}

bool ConsoleMenu::execute(int choice) {
    switch (choice) {
        case kExitChoice: return false;
        case kListRooms: listFreeRooms(); break;
        case kRegisterGuest: registerGuest(); break;
        case kBook: bookRoom(); break;
        case kConfirm: confirmReservation(); break;
        case kCheckIn: checkIn(); break;
        case kCheckOut: checkOut(); break;
        case kCancel: cancelReservation(); break;
        case kReport: showReport(); break;
        default: output_ << "Unknown command\n";
    }
    return true;
}

void ConsoleMenu::printMenu() {
    output_ << "\n=== " << hotel_->getName() << " ===\n"
            << "1 Free rooms   2 Register guest   3 Book   4 Confirm\n"
            << "5 Check-in     6 Check-out        7 Cancel 8 Report   0 Exit\n";
}

void ConsoleMenu::listFreeRooms() {
    const DateRange range = readRange();
    for (const auto& room : hotel_->allRooms()) {
        if (hotel_->isRoomFree(*room, range)) {
            output_ << "Room " << room->getNumber() << " " << room->getType()->getName() << " "
                    << room->ratePerNight().format() << "/night\n";
        }
    }
}

void ConsoleMenu::registerGuest() {
    const Person person = readPerson();
    const PassportData passport = readPassport();
    const int id = nextGuestId_;
    auto card = std::make_shared<LoyaltyCard>("LC-" + std::to_string(id));
    hotel_->registerGuest(std::make_shared<Guest>(person, id, passport, card));
    ++nextGuestId_;
    output_ << "Guest registered with id " << id << "\n";
}

Person ConsoleMenu::readPerson() {
    const std::string first = askText("First name");
    const std::string last = askText("Last name");
    const int age = askInt("Age");
    const std::string phone = askText("Phone");
    const std::string email = askText("Email");
    return Person(first, last, age, ContactInfo(phone, email));
}

PassportData ConsoleMenu::readPassport() {
    const std::string number = askText("Passport number");
    const std::string country = askText("Passport country");
    return PassportData(number, country, Date(askInt("Passport expiry day")));
}

DateRange ConsoleMenu::readRange() {
    const int from = askInt("Arrival day");
    const int to = askInt("Departure day");
    return DateRange(Date(from), Date(to));
}

void ConsoleMenu::bookRoom() {
    const int guestId = askInt("Guest id");
    const int guestCount = askInt("Number of guests");
    const auto reservation = hotel_->book(guestId, guestCount, readRange());
    output_ << "Reservation " << reservation->getId() << " for room " << reservation->getRoom()->getNumber()
            << ", cost " << reservation->totalCost().format() << "\n";
}

void ConsoleMenu::confirmReservation() {
    hotel_->findReservation(askInt("Reservation id"))->confirm();
    output_ << "Reservation confirmed\n";
}

void ConsoleMenu::checkIn() {
    const auto reservation = hotel_->findReservation(askInt("Reservation id"));
    CheckInRecord record(reservation, receptionist_, Date(askInt("Today")));
    const KeyCard card = record.performCheckIn("KC-" + std::to_string(reservation->getId()));
    output_ << "Key card " << card.getCardId() << " issued until " << card.getValidity().getEnd().label() << "\n";
}

void ConsoleMenu::checkOut() {
    const auto reservation = hotel_->findReservation(askInt("Reservation id"));
    CheckOutRecord record(reservation, Date(askInt("Today")));
    const TaxRule tax("VAT", constants::kDemoTaxPercent);
    const Money amount = tax.withTax(record.performCheckOut());
    reservation->getGuest()->earnPointsForStay(reservation->getStay().nights());
    output_ << "Checked out. Total with tax: " << amount.format() << "\n";
}

void ConsoleMenu::cancelReservation() {
    const int id = askInt("Reservation id");
    const Money penalty = hotel_->cancelReservation(id, Date(askInt("Today")));
    output_ << "Cancelled. Penalty: " << penalty.format() << "\n";
}

void ConsoleMenu::showReport() {
    const OccupancyReport report(hotel_, Date(askInt("Day")));
    output_ << "Booked rooms: " << report.bookedRooms() << ", occupancy: " << report.occupancyPercent()
            << "%, expected revenue: " << report.expectedRevenue().format() << "\n";
}

std::string ConsoleMenu::askText(const std::string& prompt) {
    output_ << prompt << ": ";
    std::string line;
    if (!std::getline(input_, line)) {
        throw InputEnded{};
    }
    return line;
}

int ConsoleMenu::askInt(const std::string& prompt) { return std::stoi(askText(prompt)); }

}  // namespace hotel::cli
