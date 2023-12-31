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
#include "scope_time_logger.hpp"

namespace
{
    // idk which is better, using hash or using stringstream to get numeric representation of the id
    std::size_t getThreadNum(const std::thread::id& threadId)
    {
        // auto hash{ std::hash<std::thread::id>{} };
        // return hash(threadId);

        std::size_t       threadId_num;
        std::stringstream ss;
        ss << threadId;
        ss >> threadId_num;
        return threadId_num;
    }

    // helper function that decides whether to execute the task immediately or enqueue it.
    static inline void runTask(window::Window* windowPtr, std::function<void()>&& func)
    {
        // If the Window is attached to the same thread as the windowManager,
        // execute the task immediately, else enqueue the task.
        const auto& windowManager{ window::WindowManager::getInstance()->get() };
        if (windowPtr->getAttachedThreadId() == windowManager.getAttachedThreadId()) {
            func();
        } else {
            windowPtr->enqueueTask(std::move(func));
        }
    }
}

namespace window
{
    void Window::framebufferSizeCallback(GLFWwindow* window, int width, int height)
    {
        Window* windowWindow{ static_cast<Window*>(glfwGetWindowUserPointer(window)) };
        if (windowWindow == nullptr) { return; }

        runTask(windowWindow, [windowWindow, width, height] {
            if (windowWindow->m_framebufferSize) {
                windowWindow->m_framebufferSize(*windowWindow, width, height);
            }
            windowWindow->setWindowSize(width, height);
        });
    }

    void Window::keyCallback(GLFWwindow* window, int key, int /* scancode */, int action, int mods)
    {
        auto* windowWindow{ static_cast<Window*>(glfwGetWindowUserPointer(window)) };
        if (windowWindow == nullptr) { return; }

        runTask(windowWindow, [windowWindow, key, action, mods] {
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

        runTask(windowWindow, [windowWindow, xPos, yPos] {
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

        runTask(windowWindow, [windowWindow, xOffset, yOffset] {
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
        , m_attachedThreadId{ std::nullopt }
    {
        useHere();
        if (!m_contextInitialized) {
            glbinding::initialize(static_cast<glbinding::ContextHandle>(m_id), glfwGetProcAddress, true);    // only resolve functions that are actually used (lazy)
            m_contextInitialized = true;

            glfwSetFramebufferSizeCallback(m_windowHandle, Window::framebufferSizeCallback);
            glfwSetKeyCallback(m_windowHandle, Window::keyCallback);
            glfwSetCursorPosCallback(m_windowHandle, Window::cursorPosCallback);
            glfwSetScrollCallback(m_windowHandle, Window::scrollCallback);
        }
        setVsync(m_vsync);
        glfwSetWindowUserPointer(m_windowHandle, this);
        unUse();
    }

    Window::Window(Window&& other)
        : m_id{ other.m_id }
        , m_contextInitialized{ other.m_contextInitialized }
        , m_windowHandle{ other.m_windowHandle }
        , m_properties{ std::move(other.m_properties) }
        , m_vsync{ other.m_vsync }
        , m_keyMap{ std::move(other.m_keyMap) }
        , m_cursorPosCallback{ std::move(other.m_cursorPosCallback) }
        , m_scrollCallback{ std::move(other.m_scrollCallback) }
        , m_taskQueue{ std::move(other.m_taskQueue) }
        , m_lastFrameTime{ other.m_lastFrameTime }
        , m_deltaTime{ other.m_deltaTime }
        , m_attachedThreadId{ other.m_attachedThreadId }
    {
        glfwSetWindowUserPointer(m_windowHandle, this);
        other.m_id                = 0;
        other.m_windowHandle      = nullptr;
        other.m_keyMap            = {};
        other.m_cursorPosCallback = nullptr;
        other.m_scrollCallback    = nullptr;
    }

    Window& Window::operator=(Window&& other)
    {
        if (this != &other) {
            m_id                 = other.m_id;
            m_contextInitialized = other.m_contextInitialized;
            m_windowHandle       = other.m_windowHandle;
            m_properties         = std::move(other.m_properties);
            m_vsync              = other.m_vsync;
            m_keyMap             = std::move(other.m_keyMap);
            m_cursorPosCallback  = std::move(other.m_cursorPosCallback);
            m_scrollCallback     = std::move(other.m_scrollCallback);
            m_taskQueue          = std::move(other.m_taskQueue);
            m_lastFrameTime      = other.m_lastFrameTime;
            m_deltaTime          = other.m_deltaTime;
            m_attachedThreadId   = other.m_attachedThreadId;

            glfwSetWindowUserPointer(m_windowHandle, this);
            other.m_id                = 0;
            other.m_windowHandle      = nullptr;
            other.m_keyMap            = {};
            other.m_cursorPosCallback = nullptr;
            other.m_scrollCallback    = nullptr;
        }
        return *this;
    }

    Window::~Window()
    {
        if (m_windowHandle != nullptr && m_id != 0) {
            unUse();
            glfwSetWindowUserPointer(m_windowHandle, nullptr);    // remove user pointer
            auto& windowManager{ WindowManager::getInstance()->get() };
            windowManager.requestDeleteWindow(m_id);
        } else {
            // window is in invalid state (probably moved)
        }
    }

    void Window::useHere()
    {
        if (!m_attachedThreadId.has_value()) {
            // no thread attached, attach to this thread

            m_attachedThreadId = std::this_thread::get_id();

            std::cout << std::format(
                "INFO: [Window] Context ({} | {:#x}) attached (+) [thread: {:#x}]\n",
                m_id,
                (std::size_t)m_windowHandle,
                getThreadNum(*m_attachedThreadId)
            );

            glfwMakeContextCurrent(m_windowHandle);

        } else if (m_attachedThreadId == std::this_thread::get_id()) {

            // same thread, do nothing

        } else {
            // different thread, cannot attach

            std::cout << std::format(
                "WARNING: [Window] Context ({} | {:#x}) already attached to another thread [{:#x}], cannot attach to this thread [{:#x}].\n",
                m_id,
                (std::size_t)m_windowHandle,
                getThreadNum(*m_attachedThreadId),
                getThreadNum(std::this_thread::get_id())
            );

            assert(false && "Context already attached to another thread");
        }
    }

    void Window::unUse()
    {
        glfwMakeContextCurrent(nullptr);
        if (m_attachedThreadId.has_value()) {
            std::cout << std::format(
                "INFO: [Window] Context ({} | {:#x}) detached (-) [thread: {:#x}]\n",
                m_id,
                (std::size_t)m_windowHandle,
                getThreadNum(*m_attachedThreadId)
            );
        }

        m_attachedThreadId.reset();
    }

    Window& Window::setVsync(bool value)
    {
        m_vsync = value;
        glfwSwapInterval(value);    // 0 for immediate updates, 1 for updates synchronized with the vertical retrace
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
        windowManager.enqueueWindowTask(m_id, [this] {
            glfwSetWindowTitle(m_windowHandle, m_properties.m_title.c_str());
        });
    }

    void Window::run(std::function<void()>&& func)
    {
        for (std::lock_guard lock{ m_windowMutex }; !glfwWindowShouldClose(m_windowHandle);) {
            PRETTY_FUNCTION_TIME_LOG_WITH_ARG("loop");

            updateDeltaTime();
            processInput();
            processQueuedTasks();

            func();
            glfwSwapBuffers(m_windowHandle);
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
        std::cout << std::format("INFO: [Window] Window ({} | {:#x}) requested to close\n", m_id, (std::size_t)m_windowHandle);
    }

    double Window::getDeltaTime()
    {
        return m_deltaTime;
    }

    Window& Window::setCaptureMouse(bool value)
    {
        m_captureMouse = value;
        auto& windowManager{ WindowManager::getInstance()->get() };
        windowManager.enqueueTask([this] {
            if (m_captureMouse) {
                glfwGetCursorPos(m_windowHandle, &m_properties.m_cursorPos.x, &m_properties.m_cursorPos.y);    // prevent sudden jump when cursor first captured
                glfwSetInputMode(m_windowHandle, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            } else {
                glfwSetInputMode(m_windowHandle, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            }
        });
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

    Window& Window::setFramebuffersizeCallback(FramebufferSizeCallbackFun&& func)
    {
        m_framebufferSize = func;
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
        PRETTY_FUNCTION_TIME_LOG();

        // TODO: move this part to 'main thread' (glfwGetKey must be called from main thread [for now it's okay, idk why tho])
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
        auto mods{ getMods() };

        // continuous key input
        for (auto& [key, handler] : m_keyMap) {
            auto& [hmod, haction, hfun]{ handler };
            if (haction != KeyActionType::CONTINUOUS) { continue; }
            if (glfwGetKey(m_windowHandle, key) != GLFW_PRESS) { continue; }

            if (mods & hmod || hmod == 0) {    // modifier match or don't have any modifier
                hfun(*this);
            }
        }
    }

    void Window::processQueuedTasks()
    {
        PRETTY_FUNCTION_TIME_LOG();

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
