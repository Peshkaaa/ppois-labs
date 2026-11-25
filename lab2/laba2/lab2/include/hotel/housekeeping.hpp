#pragma once

#include <memory>
#include <string>

#include "hotel/people.hpp"
#include "hotel/rooms.hpp"

namespace hotel {

class InventoryItem {
public:
    InventoryItem(std::string name, int quantity, int minimumLevel);
    const std::string& getName() const { return name_; }
    int getQuantity() const { return quantity_; }
    void consume(int amount);
    void restock(int amount);
    bool needsRestock() const;

private:
    std::string name_;
    int quantity_;
    int minimumLevel_;
};

class Housekeeper : public Employee {
public:
    Housekeeper(Employee base, int assignedFloor);
    int getAssignedFloor() const { return assignedFloor_; }
    int getTasksDone() const { return tasksDone_; }
    void assignFloor(int floor);
    void recordCompletedTask();
    bool isAssignedTo(int floor) const;

private:
    int assignedFloor_;
    int tasksDone_;
};

class CleaningTask {
public:
    CleaningTask(std::shared_ptr<Room> room, std::shared_ptr<Housekeeper> housekeeper);
    bool isDone() const { return done_; }
    void complete();

private:
    std::shared_ptr<Room> room_;
    std::shared_ptr<Housekeeper> housekeeper_;
    bool done_;
};

class Technician : public Employee {
public:
    Technician(Employee base, std::string specialty);
    const std::string& getSpecialty() const { return specialty_; }
    int getRequestsClosed() const { return requestsClosed_; }
    bool canHandle(const std::string& category) const;
    void recordClosedRequest();

private:
    std::string specialty_;
    int requestsClosed_;
};

class MaintenanceRequest {
public:
    MaintenanceRequest(int id, std::shared_ptr<Room> room, std::string description);
    int getId() const { return id_; }
    bool isResolved() const { return resolved_; }
    const Money& getCost() const { return cost_; }
    void assign(const std::shared_ptr<Technician>& technician);
    void resolve(const Money& cost);

private:
    int id_;
    std::shared_ptr<Room> room_;
    std::string description_;
    std::shared_ptr<Technician> technician_;
    bool resolved_;
    Money cost_;
};

class LostItem {
public:
    LostItem(std::string description, std::shared_ptr<Room> foundInRoom, Date foundDate);
    bool isClaimed() const { return claimed_; }
    void claim();
    int daysStored(const Date& today) const;
    bool isExpired(const Date& today) const;

private:
    std::string description_;
    std::shared_ptr<Room> foundInRoom_;
    Date foundDate_;
    bool claimed_;
};

}  // namespace hotel
