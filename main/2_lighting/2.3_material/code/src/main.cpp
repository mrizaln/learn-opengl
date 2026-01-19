#include <format>
#include <iostream>
#include <map>
#include <thread>
#include <vector>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "common/old/window.hpp"
#include "common/old/window_manager.hpp"
#include "common/old/scope_time_logger.hpp"

#include "app.hpp"
#include "app_various_materials.hpp"

static constexpr int         DEFAULT_WINDOW_WIDTH  = 800;
static constexpr int         DEFAULT_WINDOW_HEIGHT = 600;
static constexpr std::string DEFAULT_WINDOW_NAME   = "LearnOpenGL";

int main()
{
    using namespace window;

    util::ScopeTimeLogger::start();

    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW\n";
        return 1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    if (!WindowManager::createInstance()) {
        std::cerr << "Failed to create WindowManager instance\n";
        glfwTerminate();
        return 1;
    }

    auto& windowManager{ WindowManager::getInstance()->get() };

    auto         window1{ windowManager.createWindow(DEFAULT_WINDOW_NAME, DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT).value() };    // ignore possible nullopt :>
    std::jthread window1Thread{
        [window = std::move(window1)]() mutable {
            window.useHere();

            App::createInstance(window);
            auto& app{ App::getInstance()->get() };
            app.init();
            app.readDeviceInformation();

            window.run([&app] {
                SCOPE_TIME_LOG("Window::run lambda (window1)");

                app.render();
            });

            App::destroyInstance();
        }
    };

    auto         window2{ windowManager.createWindow(DEFAULT_WINDOW_NAME, DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT).value() };    // ignore possible nullopt :>
    std::jthread window2Thread{
        [window = std::move(window2)]() mutable {
            window.useHere();

            App2::createInstance(window);
            auto& app{ App2::getInstance()->get() };
            app.init();
            app.readDeviceInformation();

            window.run([&app] {
                SCOPE_TIME_LOG("Window::run lambda (window2)");

                app.render();
            });

            App2::destroyInstance();
        }
    };

    while (windowManager.hasWindowOpened()) {
        windowManager.pollEvents(120_fps);
    }

    WindowManager::destroyInstance();

    glfwTerminate();

    if (auto records{ util::ScopeTimeLogger::read(util::ScopeTimeLogger::ScopeStatus::ACTIVE_AND_INACTIVE) }; records) {
        std::cout << "\n>>> ScopeTimeLogger records:\n";
        for (auto& [name, time, threadId, activity] : *records) {
            std::cout << std::format("[{:#x}]: {:.3f} ms ({} | {})\n", threadId, time, name, activity ? "active" : "inactive");
        }
    }

    return 0;
}
