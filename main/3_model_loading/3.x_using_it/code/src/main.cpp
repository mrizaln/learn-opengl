#include <exception>
#include <iostream>

#include "app.hpp"

int main()
{
    try {
        App::init();
    } catch (std::exception& e) {
        std::cerr << "FATAL: " << e.what() << '\n';
        std::cerr << "Terminating...\n";
        App::deinit();
        return 1;
    }

    App::run();
    App::deinit();

    return 0;
}
