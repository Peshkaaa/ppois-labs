#include "fixtures.hpp"
#include "test_framework.hpp"

using namespace fixtures;

TEST_CASE(person_basics) {
    const Person p = person();
    CHECK(p.fullName() == "Ivan Ivanov");
    CHECK(p.isAdult());
    CHECK(p.initials() == "II");
    CHECK(p.describe() == "Person Ivan Ivanov");
    CHECK(p.getAge() == 30 && p.getFirstName() == "Ivan" && p.getLastName() == "Ivanov");
    CHECK_THROWS_AS(Person("", "x", 1, contact()), InvalidDataException);
    CHECK_THROWS_AS(Person("x", "", 1, contact()), InvalidDataException);
    CHECK_THROWS_AS(Person("x", "y", -1, contact()), InvalidDataException);
}

TEST_CASE(passport_checks) {
    const PassportData passport("AB1234567", "BY", Date(100));
    CHECK(!passport.isExpiredOn(Date(100)));
    CHECK(passport.isExpiredOn(Date(101)));
    CHECK(passport.maskedNumber() == "******567");
    passport.ensureValidOn(Date(50));
    CHECK_THROWS_AS(passport.ensureValidOn(Date(200)), ExpiredDocumentException);
    CHECK_THROWS_AS(PassportData("", "BY", Date(1)), InvalidDataException);
    CHECK(PassportData("12", "BY", Date(1)).maskedNumber() == "*2");
}

TEST_CASE(loyalty_card_tiers) {
    LoyaltyCard card("LC");
    CHECK(card.discountPercent() == 0 && card.tierName() == "Standard");
    card.earnPoints(500);
    CHECK(card.discountPercent() == 5 && card.tierName() == "Silver");
    card.earnPoints(1500);
    CHECK(card.discountPercent() == 10 && card.tierName() == "Gold");
    card.redeemPoints(1600);
    CHECK(card.getTier() == LoyaltyTier::Standard && card.getPoints() == 400);
    CHECK_THROWS_AS(card.redeemPoints(9999), InsufficientFundsException);
    CHECK_THROWS_AS(card.earnPoints(0), InvalidDataException);
    CHECK_THROWS_AS(LoyaltyCard(""), InvalidDataException);
}

TEST_CASE(guest_behaviour) {
    auto g = guest(7);
    g->addPreference("quiet");
    g->addPreference("quiet");
    CHECK(g->getPreferences().size() == 1 && g->hasPreference("quiet") && !g->hasPreference("high floor"));
    CHECK(!g->isVip());
    g->earnPointsForStay(40);
    CHECK(g->isVip());
    CHECK(g->describe() == "Guest Ivan Ivanov #7");
    CHECK_THROWS_AS(g->addPreference(""), InvalidDataException);
    CHECK_THROWS_AS(g->earnPointsForStay(0), InvalidDataException);
    CHECK_THROWS_AS(Guest(person(), 0, PassportData("A", "B", Date(1)), std::make_shared<LoyaltyCard>("L")), InvalidDataException);
    CHECK_THROWS_AS(Guest(person(), 1, PassportData("A", "B", Date(1)), nullptr), InvalidDataException);
}

TEST_CASE(employee_behaviour) {
    Employee e = employee();
    e.raiseSalary(10);
    CHECK(e.getSalary().getCents() == 110000);
    CHECK(e.daysEmployed(Date(30)) == 30);
    e.startShift();
    CHECK(e.isOnDuty());
    CHECK_THROWS_AS(e.startShift(), EmployeeNotAvailableException);
    e.endShift();
    CHECK_THROWS_AS(e.endShift(), InvalidStatusTransitionException);
    CHECK(e.describe() == "Employee Petr Ivanov (Staff)");
    CHECK_THROWS_AS(e.raiseSalary(0), InvalidDataException);
    CHECK_THROWS_AS(Employee(person(), 0, "x", Money(1), Date(0)), InvalidDataException);
    CHECK_THROWS_AS(Employee(person(), 1, "", Money(1), Date(0)), InvalidDataException);
    CHECK(e.getEmployeeId() == 1 && e.getPosition() == "Staff" && e.getHireDate().getDayNumber() == 0);
}

TEST_CASE(receptionist_languages_and_desk) {
    auto r = receptionist();
    r->addLanguage("en");
    r->addLanguage("en");
    CHECK(r->speaksLanguage("en") && !r->speaksLanguage("de"));
    r->assignDesk(3);
    CHECK(r->getDeskNumber() == 3);
    CHECK_THROWS_AS(r->addLanguage(""), InvalidDataException);
    CHECK_THROWS_AS(r->assignDesk(0), InvalidDataException);
    CHECK_THROWS_AS(Receptionist(employee(), 0), InvalidDataException);
}

TEST_CASE(manager_team_and_bonus) {
    Manager manager(employee(20), "Front", Money(1000));
    auto member = std::make_shared<Employee>(employee(21));
    manager.addSubordinate(member);
    manager.addSubordinate(member);
    CHECK(manager.teamSize() == 1 && manager.manages(*member));
    CHECK(manager.awardBonus(*member, Money(400)).getCents() == 400);
    CHECK(manager.getBonusBudget().getCents() == 600);
    CHECK_THROWS_AS(manager.awardBonus(*member, Money(5000)), InsufficientFundsException);
    CHECK_THROWS_AS(manager.awardBonus(employee(99), Money(1)), EmployeeNotAvailableException);
    CHECK_THROWS_AS(manager.addSubordinate(nullptr), InvalidDataException);
    CHECK_THROWS_AS(Manager(employee(), "", Money(1)), InvalidDataException);
    CHECK(manager.getDepartment() == "Front");
}

TEST_CASE(amenity_and_bed) {
    const Amenity wifi("Wifi", Money(0));
    CHECK(wifi.isComplimentary());
    CHECK(Amenity("Spa", Money(500)).chargeForNights(3).getCents() == 1500);
    CHECK(Amenity("Spa", Money(500)).describe() == "Spa (5.00/day)");
    CHECK_THROWS_AS(Amenity("", Money(1)), InvalidDataException);
    const Bed bed(BedKind::King, 2);
    CHECK(bed.fitsGuests(2) && !bed.fitsGuests(3) && !bed.fitsGuests(0));
    CHECK(bed.kindName() == "King" && Bed(BedKind::Double, 2).kindName() == "Double" && Bed(BedKind::Single, 1).kindName() == "Single");
    CHECK_THROWS_AS(Bed(BedKind::Single, 0), InvalidDataException);
}

TEST_CASE(room_type_pricing) {
    auto type = roomType(3);
    CHECK(type->totalBedCapacity() == 3);
    CHECK(type->amenitiesDailyCost().getCents() == 500);
    CHECK(type->hasAmenity("Wifi") && !type->hasAmenity("Pool"));
    CHECK(type->canHost(3) && !type->canHost(4) && !type->canHost(0));
    CHECK(type->nightlyRate().getCents() == 10500);
    CHECK_THROWS_AS(RoomType("", 1, Money(1)), InvalidDataException);
    CHECK(type->getName() == "Standard" && type->getCapacity() == 3 && type->getBaseRate().getCents() == 10000);
}

TEST_CASE(room_state_machine) {
    auto r = room();
    CHECK(r->isAvailable() && r->statusName() == "Available");
    r->occupy();
    CHECK(r->statusName() == "Occupied");
    CHECK_THROWS_AS(r->occupy(), RoomNotAvailableException);
    CHECK_THROWS_AS(r->sendToMaintenance(), InvalidStatusTransitionException);
    r->release();
    CHECK(r->statusName() == "Cleaning");
    CHECK_THROWS_AS(r->release(), InvalidStatusTransitionException);
    r->markAvailable();
    r->sendToMaintenance();
    CHECK(r->statusName() == "Maintenance");
    r->markAvailable();
    CHECK_THROWS_AS(r->markAvailable(), InvalidStatusTransitionException);
    CHECK(r->ratePerNight().getCents() == 10500);
    CHECK_THROWS_AS(Room(0, 1, roomType()), InvalidDataException);
    CHECK_THROWS_AS(Room(1, 1, nullptr), InvalidDataException);
}

TEST_CASE(floor_rooms) {
    Floor floor(1);
    floor.addRoom(room(101));
    floor.addRoom(room(102));
    floor.findRoom(101)->occupy();
    CHECK(floor.availableRooms().size() == 1 && floor.occupiedCount() == 1);
    CHECK_THROWS_AS(floor.findRoom(999), RoomNotFoundException);
    CHECK_THROWS_AS(floor.addRoom(std::make_shared<Room>(201, 2, roomType())), InvalidDataException);
    CHECK_THROWS_AS(floor.addRoom(nullptr), InvalidDataException);
    CHECK(floor.getRooms().size() == 2 && floor.getNumber() == 1);
}

TEST_CASE(key_card_validity) {
    auto r = room();
    KeyCard card("KC", r, range(10, 12));
    CHECK(card.isValidOn(Date(10)) && !card.isValidOn(Date(12)));
    CHECK(card.opens(*r, Date(11)) && !card.opens(*room(202), Date(11)));
    card.extend(Date(15));
    CHECK(card.isValidOn(Date(14)));
    card.deactivate();
    CHECK(!card.isActive() && !card.isValidOn(Date(10)));
    CHECK_THROWS_AS(card.extend(Date(1)), InvalidDateRangeException);
    CHECK_THROWS_AS(KeyCard("", r, range()), InvalidDataException);
    CHECK(card.getCardId() == "KC");
}
