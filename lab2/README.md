# Лабораторная работа №2 — «Гостиница»

Крупный ООП-проект на C++20: модель гостиницы (номера, бронирование, заселение и
выселение, счета и платежи, персонал, ресторан, парковка, спа, конференц-залы).

## Сборка, тесты, запуск

```bash
make            # сборка консольного приложения -> build/hotel_cli
make test       # сборка и запуск unit-тестов
./build/hotel_cli
```

Или через CMake:

```bash
cmake -S . -B build-cmake -DCMAKE_BUILD_TYPE=Release
cmake --build build-cmake
ctest --test-dir build-cmake --output-on-failure
```

## Структура

- `include/hotel/*.hpp` — объявления классов (интерфейс), `src/*.cpp` — реализации;
- `cli/` — консольное меню (`ConsoleMenu`), отделено от предметной области и принимает
  потоки ввода/вывода, поэтому покрывается тестами; `app/main.cpp` — точка входа;
- `tests/` — unit-тесты на собственном мини-фреймворке (без внешних зависимостей);
- `tools/gen_readme.py` — генератор этого файла (цифры считаются из заголовков).

## Соответствие требованиям

| Требование | Нужно | Есть |
|---|---|---|
| Классы | ≥ 50 | 55 |
| Поля | ≥ 150 | 177 |
| Поведения (методы с логикой, без get/set и inline-доступа) | ≥ 100 | 194 |
| Ассоциации (класс как поле или параметр метода) | ≥ 30 | 106 |
| Собственные исключения | ≥ 12 | 15 |

## Сводная таблица классов

Формат: `Класс | число полей | число методов | связанные классы`.


| Класс | Поля | Методы | Ассоциации (связанные классы) |
|---|---|---|---|
| Address | 4 | 2 | — |
| Amenity | 2 | 3 | Money |
| Bed | 2 | 2 | — |
| CancellationPolicy | 2 | 2 | Date, Money |
| CheckInRecord | 3 | 2 | Date, KeyCard, Receptionist, Reservation |
| CheckOutRecord | 3 | 4 | Date, Money, Reservation |
| CleaningTask | 3 | 1 | Housekeeper, Room |
| Complaint | 5 | 3 | Guest, Manager, Money |
| ConferenceBooking | 4 | 2 | ConferenceHall, DateRange, Guest, Money |
| ConferenceHall | 3 | 2 | Money |
| ContactInfo | 2 | 3 | — |
| Date | 1 | 5 | — |
| DateRange | 2 | 3 | Date |
| Deposit | 3 | 2 | Invoice, Money, Reservation |
| Employee | 5 | 5 | Date, Money |
| Floor | 2 | 4 | Room |
| Guest | 4 | 5 | LoyaltyCard, PassportData |
| Hotel | 7 | 11 | Address, CancellationPolicy, Date, DateRange, Floor, Guest, Money, Reservation, Room |
| Housekeeper | 2 | 3 | — |
| InventoryItem | 3 | 3 | — |
| Invoice | 5 | 8 | Guest, InvoiceItem, Money, Payment, TaxRule |
| InvoiceItem | 3 | 2 | Money |
| KeyCard | 4 | 4 | Date, DateRange, Room |
| LostItem | 4 | 3 | Date, Room |
| LoyaltyCard | 3 | 4 | — |
| MaintenanceRequest | 6 | 2 | Money, Room, Technician |
| Manager | 3 | 4 | Money |
| Menu | 2 | 4 | MenuItem, Money |
| MenuItem | 4 | 3 | Money, TaxRule |
| Money | 1 | 7 | — |
| Notification | 3 | 2 | Guest |
| OccupancyReport | 2 | 3 | Date, Hotel, Money |
| Parking | 2 | 4 | Money, ParkingSpot |
| ParkingSpot | 3 | 2 | — |
| PassportData | 3 | 3 | Date |
| Payment | 4 | 2 | Money |
| Person | 4 | 4 | ContactInfo |
| PromoCode | 4 | 3 | Date, DateRange, Money |
| Receptionist | 2 | 3 | — |
| Refund | 3 | 2 | Money, Payment |
| Reservation | 6 | 8 | CancellationPolicy, Date, DateRange, Guest, Money, Room |
| Restaurant | 3 | 4 | Menu, Table |
| Review | 4 | 2 | Date, Guest |
| Room | 4 | 7 | Money, RoomType |
| RoomServiceOrder | 4 | 5 | InvoiceItem, MenuItem, Money, Room, Waiter |
| RoomType | 5 | 7 | Amenity, Bed, Money |
| SeasonalRate | 3 | 3 | Date, DateRange, Money |
| ShiftSchedule | 2 | 4 | Date, Employee |
| ShuttleTransfer | 4 | 3 | Guest, InvoiceItem, Money |
| SpaBooking | 3 | 3 | Guest, Money, SpaTreatment |
| SpaTreatment | 3 | 2 | Guest, Money |
| Table | 3 | 3 | — |
| TaxRule | 2 | 2 | Money |
| Technician | 2 | 2 | — |
| Waiter | 2 | 3 | Money |

## Подробное описание классов

### TaxRule
- модуль: `billing`
- поля: `name`, `ratePercent`
- методы: `taxFor`, `withTax`
- ассоциации: Money

### InvoiceItem
- модуль: `billing`
- поля: `description`, `unitPrice`, `quantity`
- методы: `total`, `increaseQuantity`
- ассоциации: Money

### Payment
- модуль: `billing`
- поля: `paymentId`, `amount`, `method`, `paid`
- методы: `process`, `methodName`
- ассоциации: Money

### Invoice
- модуль: `billing`
- поля: `invoiceId`, `guest`, `items`, `payments`, `taxRule`
- методы: `addItem`, `subtotal`, `taxAmount`, `total`, `addPayment`, `paidAmount`, `balanceDue`, `isSettled`
- ассоциации: Guest, InvoiceItem, Money, Payment, TaxRule

### Deposit
- модуль: `billing`
- поля: `reservation`, `amount`, `held`
- методы: `applyToInvoice`, `release`
- ассоциации: Invoice, Money, Reservation

### Refund
- модуль: `billing`
- поля: `payment`, `amount`, `reason`
- методы: `isFull`, `remainder`
- ассоциации: Money, Payment

### CancellationPolicy
- модуль: `booking`
- поля: `freeCancelDays`, `penaltyPercent`
- методы: `isFreeCancel`, `penaltyFor`
- ассоциации: Date, Money

### PromoCode
- модуль: `booking`
- поля: `code`, `discountPercent`, `validity`, `usesLeft`
- методы: `isValidOn`, `discountFor`, `redeem`
- ассоциации: Date, DateRange, Money

### SeasonalRate
- модуль: `booking`
- поля: `name`, `period`, `multiplierPercent`
- методы: `appliesOn`, `adjust`, `applyIfActive`
- ассоциации: Date, DateRange, Money

### Reservation
- модуль: `booking`
- поля: `id`, `guest`, `room`, `stay`, `status`, `guestCount`
- методы: `confirm`, `checkIn`, `checkOut`, `cancel`, `totalCost`, `isActive`, `overlapsWith`, `statusName`
- ассоциации: CancellationPolicy, Date, DateRange, Guest, Money, Room

### CheckInRecord
- модуль: `booking`
- поля: `reservation`, `receptionist`, `date`
- методы: `verifyDocuments`, `performCheckIn`
- ассоциации: Date, KeyCard, Receptionist, Reservation

### CheckOutRecord
- модуль: `booking`
- поля: `reservation`, `date`, `extraCharges`
- методы: `addExtraCharge`, `finalAmount`, `wasLate`, `performCheckOut`
- ассоциации: Date, Money, Reservation

### Money
- модуль: `common`
- поля: `cents`
- методы: `add`, `subtract`, `multiply`, `percentOf`, `isZero`, `isNegative`, `format`
- ассоциации: —

### Date
- модуль: `common`
- поля: `dayNumber`
- методы: `plusDays`, `daysTo`, `isBefore`, `isAfter`, `label`
- ассоциации: —

### DateRange
- модуль: `common`
- поля: `start`, `end`
- методы: `nights`, `overlaps`, `contains`
- ассоциации: Date

### Address
- модуль: `common`
- поля: `street`, `city`, `country`, `postalCode`
- методы: `formatted`, `sameCity`
- ассоциации: —

### ContactInfo
- модуль: `common`
- поля: `phone`, `email`
- методы: `emailDomain`, `maskedPhone`, `sameEmailDomain`
- ассоциации: —

### MenuItem
- модуль: `dining`
- поля: `name`, `price`, `category`, `available`
- методы: `markUnavailable`, `markAvailable`, `priceWithTax`
- ассоциации: Money, TaxRule

### Menu
- модуль: `dining`
- поля: `title`, `items`
- методы: `addItem`, `findItem`, `availableItems`, `averagePrice`
- ассоциации: MenuItem, Money

### Table
- модуль: `dining`
- поля: `number`, `seats`, `reserved`
- методы: `fits`, `reserve`, `release`
- ассоциации: —

### Restaurant
- модуль: `dining`
- поля: `name`, `menu`, `tables`
- методы: `addTable`, `seatParty`, `freeTableCount`, `largestTable`
- ассоциации: Menu, Table

### Waiter (наследует `Employee`)
- модуль: `dining`
- поля: `assignedTables`, `tips`
- методы: `assignTable`, `addTip`, `servesTable`
- ассоциации: Money

### RoomServiceOrder
- модуль: `dining`
- поля: `room`, `items`, `waiter`, `delivered`
- методы: `addItem`, `total`, `assignWaiter`, `deliver`, `toInvoiceItem`
- ассоциации: InvoiceItem, MenuItem, Money, Room, Waiter

### ParkingSpot
- модуль: `facilities`
- поля: `number`, `occupied`, `plate`
- методы: `park`, `leave`
- ассоциации: —

### Parking
- модуль: `facilities`
- поля: `spots`, `dailyFee`
- методы: `addSpot`, `freeSpots`, `parkCar`, `feeForDays`
- ассоциации: Money, ParkingSpot

### ConferenceHall
- модуль: `facilities`
- поля: `name`, `capacity`, `hourlyRate`
- методы: `fits`, `costForHours`
- ассоциации: Money

### ConferenceBooking
- модуль: `facilities`
- поля: `hall`, `organizer`, `period`, `attendees`
- методы: `totalCost`, `conflictsWith`
- ассоциации: ConferenceHall, DateRange, Guest, Money

### SpaTreatment
- модуль: `facilities`
- поля: `name`, `durationMinutes`, `price`
- методы: `priceForGuest`, `endMinute`
- ассоциации: Guest, Money

### SpaBooking
- модуль: `facilities`
- поля: `treatment`, `guest`, `startMinute`
- методы: `endMinute`, `overlaps`, `cost`
- ассоциации: Guest, Money, SpaTreatment

### ShuttleTransfer
- модуль: `facilities`
- поля: `guest`, `destination`, `price`, `confirmed`
- методы: `confirm`, `cancel`, `toInvoiceItem`
- ассоциации: Guest, InvoiceItem, Money

### Review
- модуль: `feedback`
- поля: `guest`, `rating`, `text`, `date`
- методы: `isPositive`, `summary`
- ассоциации: Date, Guest

### Notification
- модуль: `feedback`
- поля: `recipient`, `message`, `sent`
- методы: `send`, `preview`
- ассоциации: Guest

### Complaint
- модуль: `feedback`
- поля: `guest`, `description`, `resolved`, `compensation`, `manager`
- методы: `escalateTo`, `needsEscalation`, `resolve`
- ассоциации: Guest, Manager, Money

### ShiftSchedule
- модуль: `hotel`
- поля: `maxShifts`, `days`
- методы: `assign`, `shiftsOf`, `isWorking`, `cancel`
- ассоциации: Date, Employee

### Hotel
- модуль: `hotel`
- поля: `name`, `address`, `policy`, `floors`, `guests`, `reservations`, `nextReservationId`
- методы: `addFloor`, `registerGuest`, `findGuest`, `allRooms`, `findRoom`, `isRoomFree`, `findFreeRoom`, `book`, `findReservation`, `cancelReservation`, `roomCount`
- ассоциации: Address, CancellationPolicy, Date, DateRange, Floor, Guest, Money, Reservation, Room

### OccupancyReport
- модуль: `hotel`
- поля: `hotel`, `date`
- методы: `bookedRooms`, `occupancyPercent`, `expectedRevenue`
- ассоциации: Date, Hotel, Money

### InventoryItem
- модуль: `housekeeping`
- поля: `name`, `quantity`, `minimumLevel`
- методы: `consume`, `restock`, `needsRestock`
- ассоциации: —

### Housekeeper (наследует `Employee`)
- модуль: `housekeeping`
- поля: `assignedFloor`, `tasksDone`
- методы: `assignFloor`, `recordCompletedTask`, `isAssignedTo`
- ассоциации: —

### CleaningTask
- модуль: `housekeeping`
- поля: `room`, `housekeeper`, `done`
- методы: `complete`
- ассоциации: Housekeeper, Room

### Technician (наследует `Employee`)
- модуль: `housekeeping`
- поля: `specialty`, `requestsClosed`
- методы: `canHandle`, `recordClosedRequest`
- ассоциации: —

### MaintenanceRequest
- модуль: `housekeeping`
- поля: `id`, `room`, `description`, `technician`, `resolved`, `cost`
- методы: `assign`, `resolve`
- ассоциации: Money, Room, Technician

### LostItem
- модуль: `housekeeping`
- поля: `description`, `foundInRoom`, `foundDate`, `claimed`
- методы: `claim`, `daysStored`, `isExpired`
- ассоциации: Date, Room

### Person
- модуль: `people`
- поля: `firstName`, `lastName`, `age`, `contact`
- методы: `fullName`, `isAdult`, `initials`, `describe`
- ассоциации: ContactInfo

### PassportData
- модуль: `people`
- поля: `number`, `country`, `expiry`
- методы: `isExpiredOn`, `ensureValidOn`, `maskedNumber`
- ассоциации: Date

### LoyaltyCard
- модуль: `people`
- поля: `cardNumber`, `points`, `tier`
- методы: `earnPoints`, `redeemPoints`, `discountPercent`, `tierName`
- ассоциации: —

### Guest (наследует `Person`)
- модуль: `people`
- поля: `guestId`, `passport`, `loyaltyCard`, `preferences`
- методы: `addPreference`, `hasPreference`, `isVip`, `earnPointsForStay`, `describe`
- ассоциации: LoyaltyCard, PassportData

### Employee (наследует `Person`)
- модуль: `people`
- поля: `employeeId`, `position`, `salary`, `hireDate`, `onDuty`
- методы: `raiseSalary`, `startShift`, `endShift`, `daysEmployed`, `describe`
- ассоциации: Date, Money

### Receptionist (наследует `Employee`)
- модуль: `people`
- поля: `languages`, `deskNumber`
- методы: `speaksLanguage`, `addLanguage`, `assignDesk`
- ассоциации: —

### Manager (наследует `Employee`)
- модуль: `people`
- поля: `department`, `bonusBudget`, `subordinates`
- методы: `addSubordinate`, `teamSize`, `manages`, `awardBonus`
- ассоциации: Money

### Amenity
- модуль: `rooms`
- поля: `name`, `dailyCharge`
- методы: `isComplimentary`, `chargeForNights`, `describe`
- ассоциации: Money

### Bed
- модуль: `rooms`
- поля: `kind`, `capacity`
- методы: `fitsGuests`, `kindName`
- ассоциации: —

### RoomType
- модуль: `rooms`
- поля: `name`, `capacity`, `baseRate`, `beds`, `amenities`
- методы: `addBed`, `addAmenity`, `totalBedCapacity`, `amenitiesDailyCost`, `hasAmenity`, `canHost`, `nightlyRate`
- ассоциации: Amenity, Bed, Money

### Room
- модуль: `rooms`
- поля: `number`, `floor`, `type`, `status`
- методы: `isAvailable`, `occupy`, `release`, `markAvailable`, `sendToMaintenance`, `ratePerNight`, `statusName`
- ассоциации: Money, RoomType

### Floor
- модуль: `rooms`
- поля: `number`, `rooms`
- методы: `addRoom`, `findRoom`, `availableRooms`, `occupiedCount`
- ассоциации: Room

### KeyCard
- модуль: `rooms`
- поля: `cardId`, `room`, `validity`, `active`
- методы: `isValidOn`, `opens`, `deactivate`, `extend`
- ассоциации: Date, DateRange, Room

## Исключения

- `HotelException` ← `std`
- `InvalidDataException` ← `HotelException`
- `InvalidDateRangeException` ← `HotelException`
- `RoomNotAvailableException` ← `HotelException`
- `RoomNotFoundException` ← `HotelException`
- `ReservationNotFoundException` ← `HotelException`
- `GuestNotFoundException` ← `HotelException`
- `InsufficientFundsException` ← `HotelException`
- `InvalidPaymentException` ← `HotelException`
- `CapacityExceededException` ← `HotelException`
- `InvalidStatusTransitionException` ← `HotelException`
- `CancellationNotAllowedException` ← `HotelException`
- `InvalidPromoCodeException` ← `HotelException`
- `EmployeeNotAvailableException` ← `HotelException`
- `ExpiredDocumentException` ← `HotelException`
