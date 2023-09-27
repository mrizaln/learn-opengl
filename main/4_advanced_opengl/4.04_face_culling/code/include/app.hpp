#ifndef APP_HPP_SDXGMECI
#define APP_HPP_SDXGMECI

#include <format>
#include <iostream>
#include <map>
#include <stdexcept>
#include <thread>
#include <vector>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "window.hpp"
#include "window_manager.hpp"
#include "scope_time_logger.hpp"

#include "scene.hpp"
#include "imgui_layer.hpp"

static constexpr int         DEFAULT_WINDOW_WIDTH  = 960;
static constexpr int         DEFAULT_WINDOW_HEIGHT = 720;
static constexpr std::string DEFAULT_WINDOW_NAME   = "LearnOpenGL";

class App
{
private:
    static inline std::unique_ptr<App> s_instance;

    window::Window m_window;
    std::jthread   m_windowThread;
    Scene          m_scene;
    ImGuiLayer     m_imgui;

    std::atomic<bool> m_running{ false };
    bool              m_imguiEnabled{ true };

public:
    static void init() noexcept(false)
    {
        if (s_instance) { return; }

        if (!glfwInit()) {
            std::cerr << "Failed to initialize GLFW\n";
            return;
        }

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        if (!window::WindowManager::createInstance()) {
            glfwTerminate();
            throw std::runtime_error{ "Failed to create WindowManager instance" };
        }

        auto& windowManager{ window::WindowManager::getInstance()->get() };

        auto window{ windowManager.createWindow(DEFAULT_WINDOW_NAME, DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT) };
        if (!window.has_value()) {
            throw std::runtime_error{ "Failed to create Window instance" };
        }

        window->useHere();
        s_instance.reset(new App{ std::move(window.value()) });
    }

    static void run() noexcept(false)
    {
        if (!s_instance) { throw std::runtime_error{ "No instance created" }; }

        s_instance->run_impl();
    }

    static void deinit()
    {
        s_instance.reset();

        window::WindowManager::destroyInstance();

        glfwTerminate();

        if (auto records{ util::ScopeTimeLogger::read(util::ScopeTimeLogger::ScopeStatus::ACTIVE_AND_INACTIVE) }; records) {
            std::cout << "\n>>> ScopeTimeLogger records:\n";
            for (auto& [name, time, threadId, activity] : *records) {
                std::cout << std::format("[{:#x}]: {:.3f} ms ({} | {})\n", threadId, time, name, activity ? "active" : "inactive");
            }
        }
    }

public:
    ~App() = default;

private:
    App()                      = delete;
    App(const App&)            = delete;
    App(App&&)                 = delete;
    App& operator=(const App&) = delete;
    App& operator=(App&&)      = delete;

    App(window::Window&& win)
        : m_window{ std::move(win) }
        , m_scene{ m_window }
        , m_imgui{ m_window, m_scene }
    {
        m_scene.readDeviceInformation();

        // something like debug tools in minecraft
        m_window.addKeyEventHandler(GLFW_KEY_F3, 0, window::Window::KeyActionType::CALLBACK, [this](window::Window&) {
            m_imguiEnabled = !m_imguiEnabled;
        });
    }

private:
    void run_impl()
    {
        util::ScopeTimeLogger::start();

        m_running      = true;
        m_windowThread = std::jthread{
            [this]() {
                m_window.useHere();

                m_scene.init();
                m_window.run([this] {
                    m_scene.render();
                    if (m_imguiEnabled) { m_imgui.render(); }
                });

                m_running = false;
            }
        };

        auto& windowManager{ window::WindowManager::getInstance()->get() };
        while (windowManager.hasWindowOpened() && m_running) {
            PRETTY_FUNCTION_TIME_LOG_WITH_ARG("pollEvents");

            using window::operator""_fps;
            windowManager.pollEvents(120_fps);
        }
    }
};

#endif /* end of include guard: APP_HPP_SDXGMECI */
