#pragma once

#include <memory>

#include "hotel/hotel.hpp"

namespace hotel::cli {

struct DemoData {
    std::shared_ptr<Hotel> hotel;
    std::shared_ptr<Receptionist> receptionist;
};

DemoData createDemoData();

}  // namespace hotel::cli
