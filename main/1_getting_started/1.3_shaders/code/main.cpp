#include "app.hpp"

int main()
{
    if (!App::createInstance()) {
        return 1;
    }

    auto& app{ App::getInstance().value().get() };
    app.main();
    App::destroyInstance();

    return 0;
}
