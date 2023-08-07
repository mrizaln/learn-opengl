#define USE_TEXTURE_UNIT

#ifdef USE_TEXTURE_UNIT
#    include "app_with_texture_unit.hpp"
#else
#    include "app.hpp"
#endif

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
