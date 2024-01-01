#ifndef APP_HPP_51L6TBMO
#define APP_HPP_51L6TBMO

#include <memory>
#include <iostream>

// This also include GLFW
#include "window.hpp"
#include "scene.hpp"

// Unlike in previous chapter, this one is single threaded one
class App
{
private:
    static inline std::unique_ptr<App> s_instance;

private:
    window::Window m_window;
    Scene          m_scene;

public:
    static void init() noexcept(false)
    {
        if (s_instance) { return; }

        glfwSetErrorCallback([](int error, const char* description) {
            std::cerr << "GLFW error " << error << ": " << description << '\n';
        });

        if (!glfwInit()) {
            throw std::runtime_error("Failed to initialize GLFW");
        }

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        if (!window::WindowManager::createInstance()) {
            glfwTerminate();
            throw std::runtime_error("Failed to create WindowManager instance");
        }

        auto& windowManager{ window::WindowManager::getInstance()->get() };
        auto  window{ windowManager.createWindow("LearnOpenGL", 1280, 720).value() };    // ignore error for now :)

        // Use the context before creating the App instance
        window.useHere();

        getDeviceInformation();

        s_instance.reset(new App{ std::move(window) });
    }

    // Possibly throw when App not initialized
    static void run() noexcept(false)
    {
        if (!s_instance) { throw std::runtime_error("App not initialized"); }

        s_instance->run_impl();
    }

    static void deinit()
    {
        s_instance.reset();
        window::WindowManager::destroyInstance();
        glfwTerminate();
    }

private:
    static void getDeviceInformation()
    {
        auto vendor{ gl::glGetString(gl::GL_VENDOR) };        // Returns the vendor
        auto renderer{ gl::glGetString(gl::GL_RENDERER) };    // Returns a hint to the model
        std::cout << "INFO: [Device] " << renderer << '\n';
        std::cout << "INFO: [Vendor] " << vendor << '\n';
    }

private:
    App(window::Window&& window)
        : m_window{ std::move(window) }
        , m_scene{ m_window }
    {
        using KA = window::Window::KeyActionType;

        m_window
            .setVsync(true)
            .addKeyEventHandler(GLFW_KEY_A, GLFW_MOD_ALT, KA::CALLBACK, [](window::Window&) {
                std::cout << "HI\n";
            })
            .addKeyEventHandler({ GLFW_KEY_ESCAPE, GLFW_KEY_Q }, 0, KA::CALLBACK, [](window::Window& win) {
                win.requestClose();
            });
    }

private:
    void run_impl()
    {
        auto& windowManager{ window::WindowManager::getInstance()->get() };

        m_window.run([&] {
            m_scene.render();
            windowManager.pollEvents();

            // std::cout << "fps: " << int(1 / m_window.getDeltaTime()) << '\n';
        });
    }
};

#endif /* end of include guard: APP_HPP_51L6TBMO */
