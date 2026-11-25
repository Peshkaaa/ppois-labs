#pragma once

#include <iosfwd>
#include <memory>
#include <string>

#include "hotel/hotel.hpp"

namespace hotel::cli {

class ConsoleMenu {
public:
    ConsoleMenu(std::shared_ptr<Hotel> hotel, std::shared_ptr<Receptionist> receptionist, std::istream& input,
                std::ostream& output);
    void run();

private:
    bool step();
    bool execute(int choice);
    void printMenu();
    void listFreeRooms();
    void registerGuest();
    void bookRoom();
    void confirmReservation();
    void checkIn();
    void checkOut();
    void cancelReservation();
    void showReport();
    Person readPerson();
    PassportData readPassport();
    DateRange readRange();
    std::string askText(const std::string& prompt);
    int askInt(const std::string& prompt);

    std::shared_ptr<Hotel> hotel_;
    std::shared_ptr<Receptionist> receptionist_;
    std::istream& input_;
    std::ostream& output_;
    int nextGuestId_;
};

}  // namespace hotel::cli
