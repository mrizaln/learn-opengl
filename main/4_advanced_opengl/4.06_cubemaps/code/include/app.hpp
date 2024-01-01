#ifndef APP_HPP_SDXGMECI
#define APP_HPP_SDXGMECI

#include <concepts>
#include <format>
#include <future>
#include <iostream>
#include <map>
#include <memory>
#include <mutex>
#include <stdexcept>
#include <thread>
#include <vector>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "window.hpp"
#include "window_manager.hpp"
#include "scope_time_logger.hpp"

#include "scene.hpp"
#include "scene2.hpp"
#include "imgui_layer.hpp"

static constexpr int         DEFAULT_WINDOW_WIDTH  = 960;
static constexpr int         DEFAULT_WINDOW_HEIGHT = 720;
static constexpr std::string DEFAULT_WINDOW_NAME   = "LearnOpenGL";

class App;

template <typename S>
    requires requires(S scene) {
        scene.init();
        scene.render();
    } && std::constructible_from<S, window::Window&>
class Task
{
public:
    friend App;

private:
    std::optional<window::Window> m_window;
    S                             m_scene;
    std::jthread                  m_thread;
    std::function<void()>         m_threadFunc;
    std::function<void()>         m_attachmentFunc;

public:
    static Task create(const std::string& name = DEFAULT_WINDOW_NAME)
    {
        auto& windowManager{ window::WindowManager::getInstance()->get() };
        auto  window{ windowManager.createWindow(name, DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT).value() };

        window.useHere();
        return Task{ std::move(window) };
    }

public:
    ~Task()                      = default;
    Task(const Task&)            = delete;
    Task& operator=(const Task&) = delete;
    Task(Task&&)                 = default;
    Task& operator=(Task&&)      = default;

private:
    Task(window::Window&& win)
        : m_window{ std::move(win) }
        , m_scene{ *m_window }
    {
        m_threadFunc = [this]() {
            m_window->useHere();
            m_scene.init();
            m_window->run([this] {
                m_scene.render();
                if (m_attachmentFunc) {
                    m_attachmentFunc();
                }
            });
            m_window.reset();
        };

        m_window->unUse();
    }

public:
    void addAttachment(std::function<void()>&& func) { m_attachmentFunc = std::move(func); }

    void run() { m_thread = std::jthread{ m_threadFunc }; }
};

class App
{
public:
    using Task1 = Task<Scene>;
    using Task2 = Task<Scene2>;

private:
    static inline std::unique_ptr<App> s_instance;

    Task1 m_task1;
    Task2 m_task2;

    ImGuiLayer m_imguiLayer;

    std::once_flag m_onceFlag;

public:
    static void init() noexcept(false)
    {
        if (s_instance) {
            return;
        }

        if (!glfwInit()) {
            throw std::runtime_error{ "Failed to initialize GLFW" };
        }

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        if (!window::WindowManager::createInstance()) {
            glfwTerminate();
            throw std::runtime_error{ "Failed to create WindowManager instance" };
        }

        s_instance.reset(new App{});
    }

    static void run() noexcept(false)
    {
        if (!s_instance) {
            throw std::runtime_error{ "No instance created" };
        }

        s_instance->run_impl();
    }

    static void deinit()
    {
        s_instance.reset();

        window::WindowManager::destroyInstance();

        glfwTerminate();
    }

public:
    ~App() = default;

    App(const App&)            = delete;
    App(App&&)                 = delete;
    App& operator=(const App&) = delete;
    App& operator=(App&&)      = delete;

private:
    App()
        : m_task1{ Task1::create("LearnOpenGL - Skybox") }
        , m_task2{ Task2::create("LearnOpenGL - Environment Mapping") }
        , m_imguiLayer{ m_task1.m_window.value(), m_task1.m_scene }
    {
        std::call_once(m_onceFlag, [this] {
            m_task1.m_window->useHere();

            // device information
            auto vendor{ glGetString(gl::GL_VENDOR) };        // Returns the vendor
            auto renderer{ glGetString(gl::GL_RENDERER) };    // Returns a hint to the model
            std::cout << '\n';
            std::cout << "Device: " << renderer << '\n';
            std::cout << "Vendor: " << vendor << '\n';
            std::cout << '\n';

            m_task1.m_window->unUse();
        });

        m_task1.addAttachment([this] { m_imguiLayer.render(); });
    }

private:
    void run_impl()
    {
        util::ScopeTimeLogger::start();

        m_task1.run();
        m_task2.run();

        auto& windowManager{ window::WindowManager::getInstance()->get() };
        while (windowManager.hasWindowOpened()) {
            PRETTY_FUNCTION_TIME_LOG_WITH_ARG("pollEvents");

            using window::operator""_fps;
            windowManager.pollEvents(120_fps);
        }
    }
};

#endif /* end of include guard: APP_HPP_SDXGMECI */
