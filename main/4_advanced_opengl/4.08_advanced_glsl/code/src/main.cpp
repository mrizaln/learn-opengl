#include "app.hpp"

int main()
{
    try {
        App::init();
    } catch (std::exception& e) {
        std::cerr << "Fatal Error: " << e.what() << '\n';
        App::deinit();
        return 1;
    }

    App::run();
    App::deinit();

    return 0;
}
