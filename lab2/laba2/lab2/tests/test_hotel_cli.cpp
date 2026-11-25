#include <sstream>

#include "console_menu.hpp"
#include "demo_data.hpp"
#include "fixtures.hpp"
#include "test_framework.hpp"

using namespace fixtures;

TEST_CASE(shift_schedule) {
    ShiftSchedule schedule(2);
    const Employee worker = employee(5);
    schedule.assign(worker, Date(1));
    CHECK(schedule.isWorking(5, Date(1)) && !schedule.isWorking(5, Date(2)) && !schedule.isWorking(6, Date(1)));
    CHECK_THROWS_AS(schedule.assign(worker, Date(1)), EmployeeNotAvailableException);
    schedule.assign(worker, Date(2));
    CHECK(schedule.shiftsOf(5) == 2 && schedule.shiftsOf(6) == 0);
    CHECK_THROWS_AS(schedule.assign(worker, Date(3)), EmployeeNotAvailableException);
    schedule.cancel(worker, Date(1));
    CHECK(schedule.shiftsOf(5) == 1);
    CHECK_THROWS_AS(schedule.cancel(worker, Date(9)), EmployeeNotAvailableException);
    CHECK_THROWS_AS(ShiftSchedule(0), InvalidDataException);
}

TEST_CASE(hotel_booking_flow) {
    auto h = hotelWithRooms();
    CHECK(h->roomCount() == 2 && h->getName() == "H" && h->findRoom(102)->getNumber() == 102);
    const auto first = h->book(1, 2, range(10, 12));
    const auto second = h->book(1, 2, range(10, 12));
    CHECK(first->getRoom()->getNumber() != second->getRoom()->getNumber());
    CHECK_THROWS_AS(h->book(1, 2, range(11, 13)), RoomNotAvailableException);
    CHECK(h->isRoomFree(*first->getRoom(), range(12, 14)));
    CHECK(h->findReservation(first->getId()) == first && h->getReservations().size() == 2);
    CHECK(h->cancelReservation(first->getId(), Date(1)).isZero());
    CHECK(h->book(1, 2, range(10, 12)) != nullptr);
    CHECK_THROWS_AS(h->findReservation(99), ReservationNotFoundException);
    CHECK_THROWS_AS(h->findGuest(99), GuestNotFoundException);
    CHECK_THROWS_AS(h->findRoom(999), RoomNotFoundException);
    CHECK_THROWS_AS(h->registerGuest(nullptr), InvalidDataException);
    CHECK_THROWS_AS(Hotel("", Address("S", "C", "K", "1"), CancellationPolicy(1, 1)), InvalidDataException);
}

TEST_CASE(occupancy_report) {
    auto h = hotelWithRooms();
    h->book(1, 2, range(10, 12));
    const OccupancyReport report(h, Date(10));
    CHECK(report.bookedRooms() == 1 && report.occupancyPercent() == 50);
    CHECK(report.expectedRevenue().getCents() == 21000);
    CHECK(OccupancyReport(h, Date(50)).bookedRooms() == 0);
    auto empty = std::make_shared<Hotel>("E", Address("S", "C", "K", "1"), CancellationPolicy(1, 1));
    CHECK(OccupancyReport(empty, Date(1)).occupancyPercent() == 0);
    CHECK_THROWS_AS(OccupancyReport(nullptr, Date(1)), InvalidDataException);
}

namespace {

std::string runMenu(const std::string& script) {
    const auto demo = hotel::cli::createDemoData();
    std::istringstream input(script);
    std::ostringstream output;
    hotel::cli::ConsoleMenu menu(demo.hotel, demo.receptionist, input, output);
    menu.run();
    return output.str();
}

const std::string kRegisterGuest = "2\nIvan\nIvanov\n25\n+375291112233\nivan@mail.test\nAB1234567\nBY\n100\n";

}  // namespace

TEST_CASE(cli_full_stay_scenario) {
    const std::string script = kRegisterGuest + "3\n1\n2\n10\n12\n4\n1\n5\n1\n10\n8\n10\n6\n1\n12\n0\n";
    const std::string text = runMenu(script);
    CHECK(text.find("Guest registered with id 1") != std::string::npos);
    CHECK(text.find("Reservation 1 for room 101") != std::string::npos);
    CHECK(text.find("Key card KC-1 issued") != std::string::npos);
    CHECK(text.find("Booked rooms: 1") != std::string::npos);
    CHECK(text.find("Total with tax: 209.00") != std::string::npos);
}

TEST_CASE(cli_free_rooms_and_cancel) {
    const std::string script = kRegisterGuest + "3\n1\n2\n10\n12\n1\n10\n12\n7\n1\n11\n0\n";
    const std::string text = runMenu(script);
    CHECK(text.find("Room 102") != std::string::npos);
    CHECK(text.find("Cancelled. Penalty: 95.00") != std::string::npos);
}

TEST_CASE(cli_reports_errors_and_unknown_commands) {
    const std::string text = runMenu("99\nabc\n3\n5\n1\n10\n12\n0\n");
    CHECK(text.find("Unknown command") != std::string::npos);
    CHECK(text.find("Input error") != std::string::npos);
    CHECK(text.find("Error: Guest 5 not found") != std::string::npos);
}

TEST_CASE(cli_stops_when_input_ends) {
    CHECK(runMenu("").find("Input ended") != std::string::npos);
}

TEST_CASE(demo_data_is_consistent) {
    const auto demo = hotel::cli::createDemoData();
    CHECK(demo.hotel->roomCount() == 6 && demo.receptionist->isOnDuty());
    CHECK(demo.hotel->findRoom(103)->getType()->getName() == "Suite");
}
