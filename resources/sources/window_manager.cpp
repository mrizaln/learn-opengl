#include <format>
#include <functional>
#include <iostream>
#include <mutex>
#include <optional>
#include <thread>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "window.hpp"
#include "window_manager.hpp"

namespace window
{
    bool WindowManager::createInstance()
    {
        if (!s_instance) {
            s_instance.reset(new WindowManager());
            return true;
        }
        return true;
    }

    [[nodiscard]]
    std::optional<std::reference_wrapper<WindowManager>> WindowManager::getInstance()
    {
        if (!s_instance) {
            return {};
        }
        return *s_instance;
    }

    void WindowManager::destroyInstance()
    {
        s_instance.reset(nullptr);
    }

    std::optional<Window> WindowManager::createWindow(const std::string& title, int width, int height)
    {
        unique_GLFWwindow glfwWindow{ glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr) };
        if (!glfwWindow) {
            std::cout << "WARNING: [WindowManager] Window creation failed\n";
            return {};
        }

        GLFWwindow* windowHandle{ glfwWindow.get() };
        std::size_t id{ ++m_windowCount };
        m_windows.emplace(id, std::move(glfwWindow));

        std::cout << std::format("INFO: [WindowManager] Window ({} | {:#x}) created\n", id, (std::size_t)windowHandle);

        return Window{ id, windowHandle, { .m_title = title, .m_width = width, .m_height = height } };
    }

    void WindowManager::requestDeleteWindow(std::size_t id)
    {
        std::scoped_lock lock{ m_queueMutex };
        if (m_windows.contains(id)) {    // accept request only for available windows
            m_windowDeleteQueue.push(id);
        }
    }

    void WindowManager::pollEvents(std::optional<std::chrono::milliseconds> msPollRate)
    {
        glfwPollEvents();
        checkTasks();

        if (msPollRate) {
            std::this_thread::sleep_for(*msPollRate);
        }
    }

    void WindowManager::waitEvents()
    {
        glfwWaitEvents();
        checkTasks();
    }

    bool WindowManager::hasWindowOpened()
    {
        return m_windows.size() != 0;
    }

    void WindowManager::enqueueWindowTask(std::size_t windowId, std::function<void()>&& task)
    {
        std::scoped_lock lock{ m_queueMutex };
        m_windowTaskQueue.emplace(windowId, std::move(task));
    }

    void WindowManager::enqueueTask(std::function<void()>&& task)
    {
        std::scoped_lock lock{ m_queueMutex };
        m_taskQueue.emplace(std::move(task));
    }

    void WindowManager::checkTasks()
    {
        std::lock_guard lock{ m_queueMutex };

        // window deletion
        while (!m_windowDeleteQueue.empty()) {
            std::size_t windowId{ m_windowDeleteQueue.front() };
            m_windowDeleteQueue.pop();

            auto found{ m_windows.find(windowId) };
            if (found == m_windows.end()) {
                continue;
            }
            std::cout << std::format("INFO: [WindowManager] Window ({} | {:#x}) deleted\n", found->first, (std::size_t)found->second.get());
            m_windows.erase(found);
        }

        // window task requests
        while (!m_windowTaskQueue.empty()) {
            auto [id, fun]{ std::move(m_windowTaskQueue.front()) };
            m_windowTaskQueue.pop();
            if (m_windows.contains(id)) {
                fun();
            } else {
                std::cout << std::format("WARNING: [WindowManager] Task for window {} failed: window has destroyed\n", id);
            }
        }

        // general task request
        while (!m_taskQueue.empty()) {
            auto fun{ std::move(m_taskQueue.front()) };
            m_taskQueue.pop();
            fun();
        }
    }
}
