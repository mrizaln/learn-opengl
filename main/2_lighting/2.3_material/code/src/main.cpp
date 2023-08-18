#include <thread>
#include <map>
#include <vector>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "window_manager.hpp"
#include "scope_time_logger.hpp"
#include "app.hpp"

static constexpr int              DEFAULT_WINDOW_WIDTH  = 800;
static constexpr int              DEFAULT_WINDOW_HEIGHT = 600;
static constexpr std::string_view DEFAULT_WINDOW_NAME   = "LearnOpenGL";

int main()
{
    using namespace window;

    util::ScopeTimeLogger::start();

    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW\n";
        return 1;
    }

    if (!WindowManager::createInstance()) {
        std::cerr << "Failed to create WindowManager instance\n";
        glfwTerminate();
        return 1;
    }

    auto& windowManager{ WindowManager::getInstance()->get() };
    auto  maybeWindow{ windowManager.createWindow(DEFAULT_WINDOW_NAME.data(), DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT) };
    if (!maybeWindow.has_value()) {
        std::cerr << "Failed to create window\n";
        return 1;
    }

    std::jthread windowThread{
        [window = std::move(maybeWindow.value())]() mutable {
            window.useHere();

            App::createInstance(window);
            auto& app{ App::getInstance()->get() };
            app.init();

            window.run([&app] {
                SCOPE_TIME_LOG("Window::run lambda");

                app.render();
            });

            App::destroyInstance();
        }
    };

    while (windowManager.hasWindowOpened()) {
        windowManager.pollEvents(120_fps);
    }

    WindowManager::destroyInstance();

    glfwTerminate();

    return 0;
}
