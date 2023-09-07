#include "app.hpp"
#include <exception>
#include <iostream>

int main()
{
    try {
        App::init();
    } catch (std::exception& e) {
        std::cerr << "ERROR: " << e.what() << '\n';
        return 1;
    }

    App::run();
    App::deinit();

    return 0;
}
