#include <format>
#include <functional>
#include <initializer_list>
#include <iostream>
#include <mutex>
#include <sstream>
#include <thread>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glbinding/gl/gl.h>
#include <glbinding/glbinding.h>

#include "window.hpp"
#include "window_manager.hpp"

namespace util
{
    std::size_t getThreadId()
    {
        std::size_t       threadId;
        std::stringstream ss;
        ss << std::this_thread::get_id();
        ss >> threadId;

        return threadId;
    }
}

namespace window
{
    void Window::framebufferSizeCallback(GLFWwindow* window, int width, int height)
    {
        Window* windowWindow{ static_cast<Window*>(glfwGetWindowUserPointer(window)) };
        if (windowWindow == nullptr) { return; }

        windowWindow->enqueueTask([windowWindow, width, height] {
            windowWindow->setWindowSize(width, height);
        });
    }

    void Window::keyCallback(GLFWwindow* window, int key, int /* scancode */, int action, int mods)
    {
        auto* windowWindow{ static_cast<Window*>(glfwGetWindowUserPointer(window)) };
        if (windowWindow == nullptr) { return; }

        windowWindow->enqueueTask([windowWindow, key, action, mods] {
            auto& keyMap{ windowWindow->m_keyMap };
            auto  range{ keyMap.equal_range(key) };
            for (auto& [_, handler] : std::ranges::subrange(range.first, range.second)) {
                auto& [hmod, haction, hfun]{ handler };
                if (haction != KeyActionType::CALLBACK) { continue; }
                if (action == GLFW_RELEASE | action == GLFW_REPEAT) { continue; }    // ignore release and repeat event for now

                if (mods & hmod || hmod == 0) {    // modifier match or don't have any modifier
                    hfun(*windowWindow);
                }
            }
        });
    }

    void Window::cursorPosCallback(GLFWwindow* window, double xPos, double yPos)
    {
        auto* windowWindow{ static_cast<Window*>(glfwGetWindowUserPointer(window)) };
        if (windowWindow == nullptr) { return; }

        windowWindow->enqueueTask([windowWindow, xPos, yPos] {
            if (windowWindow->m_cursorPosCallback) {
                windowWindow->m_cursorPosCallback(*windowWindow, xPos, yPos);
            }
            windowWindow->m_properties.m_cursorPos = { xPos, yPos };
        });
    }

    void Window::scrollCallback(GLFWwindow* window, double xOffset, double yOffset)
    {
        auto* windowWindow{ static_cast<Window*>(glfwGetWindowUserPointer(window)) };
        if (windowWindow == nullptr) { return; }

        windowWindow->enqueueTask([windowWindow, xOffset, yOffset] {
            if (windowWindow->m_scrollCallback) {
                windowWindow->m_scrollCallback(*windowWindow, xOffset, yOffset);
            }
        });
    }

    // this constructor must be called only from main thread (WindowManager run in main thread)
    Window::Window(std::size_t id, GLFWwindow* handle, WindowProperties&& prop)
        : m_id{ id }
        , m_windowHandle{ handle }
        , m_properties{ prop }
    {
        useHere();
        if (!m_contextInitialized) {
            glbinding::initialize(static_cast<glbinding::ContextHandle>(m_id), glfwGetProcAddress, true);    // only resolve functions that are actually used (lazy)
            m_contextInitialized = true;

            glfwSetFramebufferSizeCallback(m_windowHandle, Window::framebufferSizeCallback);
            glfwSetKeyCallback(m_windowHandle, Window::keyCallback);
            glfwSetCursorPosCallback(m_windowHandle, Window::cursorPosCallback);
            glfwSetScrollCallback(m_windowHandle, Window::scrollCallback);

            gl::glEnable(gl::GL_DEPTH_TEST);
        }
        unUse();
        glfwSetWindowUserPointer(m_windowHandle, this);
    }

    Window::Window(Window&& other)
        : m_id{ other.m_id }
        , m_windowHandle{ other.m_windowHandle }
        , m_properties{ std::move(other.m_properties) }
        , m_keyMap{ std::move(other.m_keyMap) }
    {
        glfwSetWindowUserPointer(m_windowHandle, this);
        other.m_id           = 0;
        other.m_windowHandle = nullptr;
        other.m_keyMap       = {};
    }

    Window::~Window()
    {
        if (m_windowHandle != nullptr && m_id != 0) {
            glfwSetWindowUserPointer(m_windowHandle, nullptr);    // remove user pointer
            auto& windowManager{ WindowManager::getInstance()->get() };
            windowManager.requestDeleteWindow(m_id);
        } else {
            // window is in invalid state (probably moved)
        }
    }

    void Window::useHere()
    {
        m_attachedThreadId = util::getThreadId();
        std::cout << std::format("window {} ({:#x}) attached to thread {:#x}\n", m_id, (std::size_t)m_windowHandle, m_attachedThreadId);

        glfwMakeContextCurrent(m_windowHandle);
        // glfwSetWindowUserPointer(m_windowHandle, this);
    }

    void Window::unUse()
    {
        glfwMakeContextCurrent(nullptr);
        std::cout << std::format("window {} ({:#x}) detached from thread {:#x}\n", m_id, (std::size_t)m_windowHandle, m_attachedThreadId);
        m_attachedThreadId = util::getThreadId();
    }

    Window& Window::setClearColor(float r, float g, float b)
    {
        m_properties.m_clearColor = { r, g, b };
        gl::glClearColor(r, g, b, 1.0f);
        return *this;
    }

    void Window::setWindowSize(int width, int height)
    {
        m_properties.m_width  = width;
        m_properties.m_height = height;
    }

    void Window::updateTitle(const std::string& title)
    {
        m_properties.m_title = title;
        auto& windowManager{ WindowManager::getInstance()->get() };
        windowManager.enqueueTask(m_id, [this] {
            glfwSetWindowTitle(m_windowHandle, m_properties.m_title.c_str());
        });
    }

    void Window::run(std::function<void()>&& func)
    {
        for (std::lock_guard lock{ m_windowMutex }; !glfwWindowShouldClose(m_windowHandle);) {
            auto& prop{ getProperties() };
            gl::glViewport(0, 0, prop.m_width, prop.m_height);
            gl::glClear(gl::GL_COLOR_BUFFER_BIT | gl::GL_DEPTH_BUFFER_BIT);

            func();

            glfwSwapBuffers(m_windowHandle);

            updateDeltaTime();

            processInput();
            processQueuedTasks();
        }
    }

    void Window::enqueueTask(std::function<void()>&& func)
    {
        std::lock_guard lock{ m_queueMutex };
        m_taskQueue.push(std::move(func));
    }

    void Window::requestClose()
    {
        glfwSetWindowShouldClose(m_windowHandle, true);
        std::cout << std::format("window {} ({:#x}) requested to close\n", m_id, (std::size_t)m_windowHandle);
    }

    double Window::getDeltaTime()
    {
        return m_deltaTime;
    }

    Window& Window::setCaptureMouse(bool value)
    {
        if ((m_captureMouse = value)) {
            glfwGetCursorPos(m_windowHandle, &m_properties.m_cursorPos.x, &m_properties.m_cursorPos.y);    // prevent sudden jump when cursor first captured
            glfwSetInputMode(m_windowHandle, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        } else {
            glfwSetInputMode(m_windowHandle, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }
        return *this;
    }

    Window& Window::setCursorPosCallback(CursorPosCallbackFun&& func)
    {
        m_cursorPosCallback = func;
        return *this;
    }

    Window& Window::setScrollCallback(ScrollCallbackFun&& func)
    {
        m_scrollCallback = func;
        return *this;
    }

    Window& Window::addKeyEventHandler(KeyEvent key, KeyModifier mods, KeyActionType action, std::function<void(Window&)>&& func)
    {
        m_keyMap.emplace(key, KeyEventHandler{ .mods = mods, .action = action, .handler = func });
        return *this;
    }

    Window& Window::addKeyEventHandler(std::initializer_list<KeyEvent> keys, KeyModifier mods, KeyActionType action, std::function<void(Window&)>&& func)
    {
        for (auto key : keys) {
            m_keyMap.emplace(key, KeyEventHandler{ .mods = mods, .action = action, .handler = func });
        }
        return *this;
    }

    void Window::processInput()
    {
        const auto getMods = [win = m_windowHandle] {
            int mods{ 0 };
            mods |= glfwGetKey(win, GLFW_KEY_LEFT_SHIFT) & GLFW_MOD_SHIFT;
            mods |= glfwGetKey(win, GLFW_KEY_RIGHT_SHIFT) & GLFW_MOD_SHIFT;
            mods |= glfwGetKey(win, GLFW_KEY_LEFT_CONTROL) & GLFW_MOD_CONTROL;
            mods |= glfwGetKey(win, GLFW_KEY_RIGHT_CONTROL) & GLFW_MOD_CONTROL;
            mods |= glfwGetKey(win, GLFW_KEY_LEFT_ALT) & GLFW_MOD_ALT;
            mods |= glfwGetKey(win, GLFW_KEY_RIGHT_ALT) & GLFW_MOD_ALT;
            mods |= glfwGetKey(win, GLFW_KEY_LEFT_SUPER) & GLFW_MOD_SUPER;
            mods |= glfwGetKey(win, GLFW_KEY_RIGHT_SUPER) & GLFW_MOD_SUPER;

            return mods;
        };

        // continuous key input
        for (auto& [key, handler] : m_keyMap) {
            auto& [hmod, haction, hfun]{ handler };
            if (haction != KeyActionType::CONTINUOUS) { continue; }
            if (glfwGetKey(m_windowHandle, key) != GLFW_PRESS) { continue; }

            auto mods{ getMods() };
            if (mods & hmod || hmod == 0) {    // modifier match or don't have any modifier
                hfun(*this);
            }
        }
    }

    void Window::processQueuedTasks()
    {
        decltype(m_taskQueue) taskQueue;
        {
            std::lock_guard lock{ m_queueMutex };
            taskQueue.swap(m_taskQueue);
        }
        while (!taskQueue.empty()) {
            auto&& func{ taskQueue.front() };
            func();
            taskQueue.pop();
        }
    }

    void Window::updateDeltaTime()
    {
        double currentTime{ glfwGetTime() };
        m_deltaTime     = currentTime - m_lastFrameTime;
        m_lastFrameTime = currentTime;
    }
}
