#ifdef USE_SHADER_HEADER
#    include "app_with_shader_class.hpp"
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
