#include "demo_data.hpp"

namespace hotel::cli {

namespace {

constexpr int kFloorCount = 2;
constexpr int kRoomsPerFloor = 3;
constexpr int kFloorNumberStep = 100;
constexpr int kSuiteEveryNthRoom = 3;
constexpr long long kStandardRateCents = 8000;
constexpr long long kSuiteRateCents = 20000;
constexpr long long kBreakfastCents = 1500;
constexpr int kStandardCapacity = 2;
constexpr int kSuiteCapacity = 4;
constexpr long long kReceptionistSalaryCents = 250000;
constexpr int kReceptionistAge = 30;
constexpr int kDeskNumber = 1;
constexpr int kReceptionistId = 1;

std::shared_ptr<RoomType> makeRoomType(const std::string& name, int capacity, long long rateCents) {
    auto type = std::make_shared<RoomType>(name, capacity, Money(rateCents));
    type->addBed(Bed(BedKind::Double, capacity));
    type->addAmenity(Amenity("Breakfast", Money(kBreakfastCents)));
    return type;
}

std::shared_ptr<Receptionist> makeReceptionist() {
    Person person("Anna", "Petrova", kReceptionistAge, ContactInfo("+375291234567", "anna@hotel.test"));
    Employee employee(person, kReceptionistId, "Receptionist", Money(kReceptionistSalaryCents), Date(0));
    auto receptionist = std::make_shared<Receptionist>(employee, kDeskNumber);
    receptionist->startShift();
    return receptionist;
}

Floor makeFloor(int number, const std::shared_ptr<RoomType>& standard, const std::shared_ptr<RoomType>& suite) {
    Floor floor(number);
    for (int index = 1; index <= kRoomsPerFloor; ++index) {
        const bool isSuite = index % kSuiteEveryNthRoom == 0;
        floor.addRoom(std::make_shared<Room>(number * kFloorNumberStep + index, number, isSuite ? suite : standard));
    }
    return floor;
}

}  // namespace

DemoData createDemoData() {
    const Address address("Nezavisimosti 1", "Minsk", "Belarus", "220000");
    const CancellationPolicy policy(constants::kDemoFreeCancelDays, constants::kDemoPenaltyPercent);
    auto hotel = std::make_shared<Hotel>("Grand Hotel", address, policy);
    const auto standard = makeRoomType("Standard", kStandardCapacity, kStandardRateCents);
    const auto suite = makeRoomType("Suite", kSuiteCapacity, kSuiteRateCents);
    for (int number = 1; number <= kFloorCount; ++number) {
        hotel->addFloor(makeFloor(number, standard, suite));
    }
    return DemoData{hotel, makeReceptionist()};
}

}  // namespace hotel::cli
