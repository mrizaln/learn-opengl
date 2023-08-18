#ifndef WINDOW_MANAGER_HPP_OR5VIUQW
#define WINDOW_MANAGER_HPP_OR5VIUQW

#include <chrono>
#include <cstddef>
#include <functional>
#include <mutex>
#include <optional>
#include <queue>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

namespace window
{
    using unique_GLFWwindow = std::unique_ptr<GLFWwindow, decltype([](GLFWwindow* win) { glfwDestroyWindow(win); })>;

    // turns fps to milliseconds
    inline std::chrono::milliseconds operator""_fps(unsigned long long fps)
    {
        using namespace std::chrono_literals;
        auto duration{ std::chrono::duration_cast<std::chrono::milliseconds>(1000ms / fps) };
        return duration;
    }

    class Window;

    class WindowManager
    {
    public:
        ~WindowManager()                               = default;
        WindowManager(const WindowManager&)            = delete;
        WindowManager(WindowManager&&)                 = delete;
        WindowManager& operator=(const WindowManager&) = delete;
        WindowManager& operator=(WindowManager&&)      = delete;

        static bool                                                 createInstance();
        static std::optional<std::reference_wrapper<WindowManager>> getInstance();
        static void                                                 destroyInstance();

        // @thread_safety: call this function from the main thread only
        std::optional<Window> createWindow(const std::string& title, int width, int height);

        // this function poll events for all windows.
        // @thread_safety: call this function from the main thread only
        void pollEvents(std::chrono::milliseconds msPollRate);

        // like pollEvents, but this function will block the thread until an event is received.
        // @thread_safety: call this function from the main thread only
        void waitEvents();

        // @thread_safety: this function can be called from any thread
        void requestDeleteWindow(std::size_t id);

        // @thread_safety: this function can be called from any thread
        void enqueueTask(std::size_t windowId, std::function<void()>&& func);
        bool hasWindowOpened();

    private:
        WindowManager() = default;

        void checkTasks();

        inline static std::unique_ptr<WindowManager> s_instance{ nullptr };

        std::unordered_map<std::size_t, unique_GLFWwindow>        m_windows;
        std::size_t                                               m_windowCount;
        std::queue<std::size_t>                                   m_windowDeleteQueue;
        std::queue<std::pair<std::size_t, std::function<void()>>> m_taskQueue;

        std::mutex m_queueMutex;
    };
}

#endif /* end of include guard: WINDOW_MANAGER_HPP_OR5VIUQW */
