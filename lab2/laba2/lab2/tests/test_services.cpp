#include "fixtures.hpp"
#include "test_framework.hpp"

using namespace fixtures;

TEST_CASE(inventory_item) {
    InventoryItem towels("Towel", 10, 3);
    towels.consume(7);
    CHECK(towels.needsRestock() && towels.getQuantity() == 3);
    towels.restock(5);
    CHECK(!towels.needsRestock() && towels.getName() == "Towel");
    CHECK_THROWS_AS(towels.consume(100), InsufficientFundsException);
    CHECK_THROWS_AS(towels.restock(0), InvalidDataException);
    CHECK_THROWS_AS(InventoryItem("", 1, 1), InvalidDataException);
}

TEST_CASE(housekeeper_and_cleaning_task) {
    auto keeper = housekeeper();
    auto r = room();
    r->occupy();
    r->release();
    CleaningTask task(r, keeper);
    task.complete();
    CHECK(task.isDone() && r->isAvailable() && keeper->getTasksDone() == 1);
    CHECK_THROWS_AS(task.complete(), InvalidStatusTransitionException);
    CHECK_THROWS_AS(CleaningTask(r, housekeeper(false)).complete(), EmployeeNotAvailableException);
    keeper->assignFloor(2);
    CHECK(keeper->isAssignedTo(2) && keeper->getAssignedFloor() == 2);
    CHECK_THROWS_AS(keeper->assignFloor(0), InvalidDataException);
    CHECK_THROWS_AS(CleaningTask(nullptr, keeper), InvalidDataException);
    CHECK_THROWS_AS(CleaningTask(r, nullptr), InvalidDataException);
    CHECK_THROWS_AS(Housekeeper(employee(), 0), InvalidDataException);
}

TEST_CASE(technician_and_maintenance) {
    auto tech = technician();
    auto r = room();
    MaintenanceRequest request(1, r, "leak");
    CHECK(r->statusName() == "Maintenance");
    CHECK_THROWS_AS(request.resolve(Money(1)), EmployeeNotAvailableException);
    CHECK_THROWS_AS(request.assign(technician(false)), EmployeeNotAvailableException);
    CHECK_THROWS_AS(request.assign(nullptr), InvalidDataException);
    request.assign(tech);
    request.resolve(Money(700));
    CHECK(request.isResolved() && request.getCost().getCents() == 700 && r->isAvailable());
    CHECK(tech->getRequestsClosed() == 1 && tech->canHandle("plumbing") && !tech->canHandle("wiring"));
    CHECK_THROWS_AS(request.resolve(Money(1)), InvalidStatusTransitionException);
    CHECK_THROWS_AS(MaintenanceRequest(2, r, ""), InvalidDataException);
    CHECK(request.getId() == 1 && tech->getSpecialty() == "plumbing");
    CHECK_THROWS_AS(Technician(employee(), ""), InvalidDataException);
}

TEST_CASE(lost_item) {
    LostItem item("watch", room(), Date(10));
    CHECK(item.daysStored(Date(15)) == 5 && !item.isExpired(Date(15)) && item.isExpired(Date(101)));
    item.claim();
    CHECK(item.isClaimed());
    CHECK_THROWS_AS(item.claim(), InvalidStatusTransitionException);
    CHECK_THROWS_AS(LostItem("", room(), Date(1)), InvalidDataException);
}

TEST_CASE(menu_and_items) {
    auto soup = std::make_shared<MenuItem>("Soup", Money(500), "starter");
    auto steak = std::make_shared<MenuItem>("Steak", Money(1500), "main");
    Menu menu("Dinner");
    menu.addItem(soup);
    menu.addItem(steak);
    steak->markUnavailable();
    CHECK(menu.availableItems().size() == 1 && menu.findItem("Soup") == soup);
    steak->markAvailable();
    CHECK(steak->isAvailable() && menu.averagePrice().getCents() == 1000);
    CHECK(soup->priceWithTax(TaxRule("V", 10)).getCents() == 550 && soup->getPrice().getCents() == 500);
    CHECK(Menu("Empty").averagePrice().isZero());
    CHECK_THROWS_AS(menu.findItem("Pizza"), InvalidDataException);
    CHECK_THROWS_AS(menu.addItem(nullptr), InvalidDataException);
    CHECK_THROWS_AS(MenuItem("", Money(1), "c"), InvalidDataException);
    CHECK(menu.getTitle() == "Dinner" && soup->getName() == "Soup");
}

TEST_CASE(restaurant_tables) {
    Restaurant restaurant("Bistro", std::make_shared<Menu>("M"));
    restaurant.addTable(Table(1, 2));
    restaurant.addTable(Table(2, 4));
    CHECK(restaurant.seatParty(3) == 2 && restaurant.freeTableCount() == 1 && restaurant.largestTable() == 4);
    CHECK(restaurant.seatParty(2) == 1);
    CHECK_THROWS_AS(restaurant.seatParty(2), CapacityExceededException);
    CHECK_THROWS_AS(Restaurant("", std::make_shared<Menu>("M")), InvalidDataException);
    CHECK(restaurant.getName() == "Bistro");
}

TEST_CASE(table_reservation) {
    Table table(1, 2);
    CHECK(table.fits(2) && !table.fits(3));
    table.reserve(2);
    CHECK(table.isReserved() && table.getNumber() == 1 && table.getSeats() == 2);
    CHECK_THROWS_AS(table.reserve(2), InvalidStatusTransitionException);
    table.release();
    CHECK_THROWS_AS(table.reserve(5), CapacityExceededException);
    CHECK_THROWS_AS(Table(0, 1), InvalidDataException);
}

TEST_CASE(waiter_tips_and_tables) {
    auto w = waiter();
    w->assignTable(3);
    w->assignTable(3);
    w->addTip(Money(200));
    CHECK(w->servesTable(3) && !w->servesTable(4) && w->getTips().getCents() == 200);
    CHECK_THROWS_AS(w->assignTable(0), InvalidDataException);
    CHECK_THROWS_AS(w->addTip(Money(-1)), InvalidDataException);
}

TEST_CASE(room_service_order) {
    auto r = room();
    CHECK_THROWS_AS(RoomServiceOrder(r), InvalidStatusTransitionException);
    r->occupy();
    RoomServiceOrder order(r);
    auto soup = std::make_shared<MenuItem>("Soup", Money(500), "starter");
    order.addItem(soup);
    order.addItem(soup);
    CHECK(order.total().getCents() == 1000 && order.toInvoiceItem().total().getCents() == 1000);
    CHECK_THROWS_AS(order.deliver(), EmployeeNotAvailableException);
    order.assignWaiter(waiter());
    order.deliver();
    CHECK(order.isDelivered());
    CHECK_THROWS_AS(order.deliver(), InvalidStatusTransitionException);
    soup->markUnavailable();
    CHECK_THROWS_AS(order.addItem(soup), InvalidDataException);
    CHECK_THROWS_AS(order.addItem(nullptr), InvalidDataException);
    CHECK_THROWS_AS(order.assignWaiter(nullptr), InvalidDataException);
    CHECK_THROWS_AS(RoomServiceOrder(nullptr), InvalidDataException);
}

TEST_CASE(parking) {
    Parking parking(Money(300));
    parking.addSpot(ParkingSpot(1));
    CHECK(parking.freeSpots() == 1 && parking.parkCar("AB-1") == 1 && parking.freeSpots() == 0);
    CHECK_THROWS_AS(parking.parkCar("AB-2"), CapacityExceededException);
    CHECK(parking.feeForDays(3).getCents() == 900);
    ParkingSpot spot(2);
    CHECK_THROWS_AS(spot.leave(), InvalidStatusTransitionException);
    spot.park("X");
    CHECK(!spot.isFree() && spot.getNumber() == 2);
    CHECK_THROWS_AS(spot.park("Y"), InvalidStatusTransitionException);
    CHECK_THROWS_AS(spot.park(""), InvalidDataException);
    spot.leave();
    CHECK(spot.isFree());
    CHECK_THROWS_AS(ParkingSpot(0), InvalidDataException);
}

TEST_CASE(conference_booking) {
    auto hall = std::make_shared<ConferenceHall>("Alpha", 50, Money(1000));
    CHECK(hall->fits(50) && !hall->fits(51) && hall->costForHours(3).getCents() == 3000);
    ConferenceBooking booking(hall, guest(), range(10, 12), 40);
    CHECK(booking.totalCost(4).getCents() == 8000 && booking.getPeriod().nights() == 2);
    CHECK(booking.conflictsWith(ConferenceBooking(hall, guest(), range(11, 13), 10)));
    const auto otherHall = std::make_shared<ConferenceHall>("Beta", 50, Money(1000));
    CHECK(!booking.conflictsWith(ConferenceBooking(otherHall, guest(), range(11, 13), 10)));
    CHECK_THROWS_AS(ConferenceBooking(hall, guest(), range(), 99), CapacityExceededException);
    CHECK_THROWS_AS(ConferenceBooking(nullptr, guest(), range(), 1), InvalidDataException);
    CHECK_THROWS_AS(ConferenceBooking(hall, nullptr, range(), 1), InvalidDataException);
    CHECK_THROWS_AS(ConferenceHall("", 1, Money(1)), InvalidDataException);
    CHECK(hall->getName() == "Alpha" && hall->getCapacity() == 50);
}

TEST_CASE(spa_booking) {
    auto massage = std::make_shared<SpaTreatment>("Massage", 60, Money(10000));
    auto g = guest();
    SpaBooking first(massage, g, 600);
    CHECK(first.endMinute() == 660 && first.cost().getCents() == 10000 && first.getStartMinute() == 600);
    CHECK(first.overlaps(SpaBooking(massage, g, 630)) && !first.overlaps(SpaBooking(massage, g, 660)));
    g->earnPointsForStay(40);
    CHECK(massage->priceForGuest(*g).getCents() == 9000 && massage->getName() == "Massage");
    CHECK_THROWS_AS(SpaBooking(nullptr, g, 1), InvalidDataException);
    CHECK_THROWS_AS(SpaBooking(massage, nullptr, 1), InvalidDataException);
    CHECK_THROWS_AS(SpaBooking(massage, g, -1), InvalidDataException);
    CHECK_THROWS_AS(SpaTreatment("", 1, Money(1)), InvalidDataException);
}

TEST_CASE(shuttle_transfer) {
    ShuttleTransfer transfer(guest(), "Airport", Money(2500));
    CHECK_THROWS_AS(transfer.toInvoiceItem(), InvalidStatusTransitionException);
    CHECK_THROWS_AS(transfer.cancel(), InvalidStatusTransitionException);
    transfer.confirm();
    CHECK(transfer.isConfirmed() && transfer.toInvoiceItem().getDescription() == "Shuttle to Airport");
    CHECK_THROWS_AS(transfer.confirm(), InvalidStatusTransitionException);
    transfer.cancel();
    CHECK(!transfer.isConfirmed());
    CHECK_THROWS_AS(ShuttleTransfer(nullptr, "A", Money(1)), InvalidDataException);
    CHECK_THROWS_AS(ShuttleTransfer(guest(), "", Money(1)), InvalidDataException);
}

TEST_CASE(review_notification_complaint) {
    Review review(guest(), 5, "great", Date(1));
    CHECK(review.isPositive() && review.summary() == "Ivan Ivanov: 5/5" && review.getRating() == 5);
    CHECK(!Review(guest(), 2, "bad", Date(1)).isPositive());
    CHECK_THROWS_AS(Review(guest(), 6, "x", Date(1)), InvalidDataException);
    CHECK_THROWS_AS(Review(nullptr, 3, "x", Date(1)), InvalidDataException);
    Notification note(guest(), "Your room is ready for arrival today");
    CHECK(note.preview() == "Your room is ready f...");
    CHECK(Notification(guest(), "short").preview() == "short");
    note.send();
    CHECK(note.isSent());
    CHECK_THROWS_AS(note.send(), InvalidStatusTransitionException);
    CHECK_THROWS_AS(Notification(nullptr, "x"), InvalidDataException);
    CHECK_THROWS_AS(Notification(guest(), ""), InvalidDataException);
}

TEST_CASE(complaint_resolution) {
    Complaint complaint(guest(), "noise");
    CHECK(complaint.isOpen() && complaint.needsEscalation());
    CHECK_THROWS_AS(complaint.resolve(Money(100)), EmployeeNotAvailableException);
    complaint.escalateTo(std::make_shared<Manager>(employee(30), "Front", Money(1)));
    CHECK(!complaint.needsEscalation());
    complaint.resolve(Money(100));
    CHECK(!complaint.isOpen() && complaint.getCompensation().getCents() == 100);
    CHECK_THROWS_AS(complaint.resolve(Money(0)), InvalidStatusTransitionException);
    Complaint plain(guest(), "late");
    plain.resolve(Money(0));
    CHECK(!plain.isOpen());
    CHECK_THROWS_AS(plain.escalateTo(nullptr), InvalidDataException);
    CHECK_THROWS_AS(Complaint(nullptr, "x"), InvalidDataException);
    CHECK_THROWS_AS(Complaint(guest(), ""), InvalidDataException);
}
