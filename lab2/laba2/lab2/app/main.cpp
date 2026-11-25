#include <iostream>

#include "console_menu.hpp"
#include "demo_data.hpp"

int main() {
    const hotel::cli::DemoData demo = hotel::cli::createDemoData();
    hotel::cli::ConsoleMenu menu(demo.hotel, demo.receptionist, std::cin, std::cout);
    menu.run();
    return 0;
}
