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
            std::cout << "Window creation failed\n";
            return {};
        }

        GLFWwindow* windowHandle{ glfwWindow.get() };
        std::size_t id{ ++m_windowCount };
        m_windows.emplace(id, std::move(glfwWindow));

        std::cout << std::format("Window {} ({:#x}) created\n", id, (std::size_t)windowHandle);

        return Window{ id, windowHandle, { .m_title = title, .m_width = width, .m_height = height } };
    }

    void WindowManager::requestDeleteWindow(std::size_t id)
    {
        std::scoped_lock lock{ m_queueMutex };
        if (m_windows.contains(id)) {    // accept request only for available windows
            m_windowDeleteQueue.push(id);
        }
    }

    void WindowManager::pollEvents(std::chrono::milliseconds msPollRate)
    {
        glfwPollEvents();
        checkTasks();
        std::this_thread::sleep_for(msPollRate);
    }

    void WindowManager::pollEvents(std::size_t fpsPollRate)
    {
        using namespace std::chrono_literals;
        auto duration{ std::chrono::duration_cast<std::chrono::milliseconds>(1000ms / fpsPollRate) };
        pollEvents(duration);
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

    void WindowManager::checkTasks()
    {
        // window deletion
        while (!m_windowDeleteQueue.empty()) {
            std::size_t windowId{ m_windowDeleteQueue.front() };
            m_windowDeleteQueue.pop();

            auto found{ m_windows.find(windowId) };
            if (found == m_windows.end()) {
                continue;
            }
            std::cout << std::format("Window {} ({:#x}) deleted\n", found->first, (std::size_t)found->second.get());
            m_windows.erase(found);
        }

        // task requests
        while (!m_taskQueue.empty()) {
            auto [id, fun]{ std::move(m_taskQueue.front()) };
            m_taskQueue.pop();
            if (m_windows.contains(id)) {
                fun();
            } else {
                std::cerr << std::format("Task for window {} failed: window has destroyed\n", id);
            }
        }
    }

    void WindowManager::enqueueTask(std::size_t windowId, std::function<void()>&& task)
    {
        std::scoped_lock lock{ m_queueMutex };
        m_taskQueue.emplace(windowId, std::move(task));
    }
}
