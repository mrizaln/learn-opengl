#ifndef WINDOW_HPP_IROQWEOX
#define WINDOW_HPP_IROQWEOX

#include <functional>
#include <utility>
#include <atomic>
#include <queue>

#include <glm/glm.hpp>

#include "window_manager.hpp"

namespace window
{
    class WindowManager;

    struct WindowProperties
    {
        std::string         m_title;
        int                 m_width;
        int                 m_height;
        glm::vec3           m_clearColor;
        glm::vec<2, double> m_cursorPos;
    };

    class Window
    {
    public:
        friend WindowManager;

        using KeyEvent    = int;    // GLFW_KEY_*
        using KeyModifier = int;    // GLFW_MOD_*
        enum class KeyActionType
        {
            CALLBACK,
            CONTINUOUS,
        };
        struct KeyEventHandler
        {
            KeyModifier                  mods;
            KeyActionType                action;
            std::function<void(Window&)> handler;
        };
        using KeyMap = std::unordered_multimap<KeyEvent, KeyEventHandler>;

        using CursorPosCallbackFun = std::function<void(Window& window, double xPos, double yPos)>;
        using ScrollCallbackFun    = std::function<void(Window& window, double xOffset, double yOffset)>;

        Window(const Window&) = delete;
        Window(Window&&);
        ~Window();

        // use the context on current thread;
        void useHere();
        void unUse();
        void setWindowSize(int width, int height);
        void updateTitle(const std::string& title);
        // main rendering loop
        void    run(std::function<void()>&& func);
        void    enqueueTask(std::function<void()>&& func);
        void    requestClose();
        double  getDeltaTime();
        Window& setClearColor(float r, float g, float b);
        Window& setCaptureMouse(bool value);
        Window& setCursorPosCallback(CursorPosCallbackFun&& func);
        Window& setScrollCallback(ScrollCallbackFun&& func);

        // The function added will be called from the window thread.
        Window& addKeyEventHandler(KeyEvent key, KeyModifier mods, KeyActionType action, std::function<void(Window&)>&& func);
        Window& addKeyEventHandler(std::initializer_list<KeyEvent> keys, KeyModifier mods, KeyActionType action, std::function<void(Window&)>&& func);

        bool              isMouseCaptured() { return m_captureMouse; }
        WindowProperties& getProperties() { return m_properties; }
        GLFWwindow*       getHandle() { return m_windowHandle; }

    private:
        Window(std::size_t id, GLFWwindow* handle, WindowProperties&& prop);

        static void framebufferSizeCallback(GLFWwindow* window, int width, int height);
        static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
        static void cursorPosCallback(GLFWwindow* window, double xPos, double yPos);
        static void scrollCallback(GLFWwindow* window, double xOffset, double yOffset);

        void processInput();
        void processQueuedTasks();
        void updateDeltaTime();

        // window stuff
        std::size_t      m_id;    // imagine this is the context handle (GLFW use the same handle for the window and the context)
        bool             m_contextInitialized{ false };
        GLFWwindow*      m_windowHandle;
        WindowProperties m_properties;

        // input
        KeyMap               m_keyMap;
        CursorPosCallbackFun m_cursorPosCallback;
        ScrollCallbackFun    m_scrollCallback;

        std::queue<std::function<void()>> m_taskQueue;

        double m_lastFrameTime{ 0.0 };
        double m_deltaTime{ 0.0 };

        bool        m_captureMouse{ false };
        std::size_t m_attachedThreadId;

        mutable std::mutex m_windowMutex;
        mutable std::mutex m_queueMutex;
    };
}

#endif /* end of include guard: WINDOW_HPP_IROQWEOX */
