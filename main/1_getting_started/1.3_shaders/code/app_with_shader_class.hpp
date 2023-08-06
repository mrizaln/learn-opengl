#ifndef APP_HPP_CCU84BHI
#define APP_HPP_CCU84BHI

#include <array>
#include <atomic>
#include <cmath>
#include <cstddef>
#include <format>
#include <functional>
#include <iostream>
#include <memory>
#include <optional>
#include <string_view>
#include <type_traits>
#include <utility>

#define GLFW_INCLUDE_NONE
#include <glbinding/gl/gl.h>
#include <glbinding/glbinding.h>
#include <GLFW/glfw3.h>

#include "shader.hpp"

using namespace gl;

class App
{
public:
    static constexpr int              s_windowWidth  = 800;
    static constexpr int              s_windowHeight = 600;
    static constexpr std::string_view s_windowName   = "LearnOpenGL";

    template <typename T>
    using Triple = std::array<T, 3>;

    template <typename T = float>
    using Point = Triple<T>;

    using PosAndColor = struct
    {
        Point<float>  pos;
        Triple<float> color;
    };

    using TriangleVert = Triple<PosAndColor>;

    static constexpr TriangleVert s_triangleVertices{ {
        { .pos = { 0.5f, -0.5f, 0.0f }, .color = { 1.0f, 0.0f, 0.0f } },
        { .pos = { -0.5f, -0.5f, 0.0f }, .color = { 0.0f, 1.0f, 0.0f } },
        { .pos = { 0.0f, 0.5f, 0.0f }, .color = { 0.0f, 0.0f, 1.0f } },
    } };

private:
    static inline std::unique_ptr<App> s_instance{ nullptr };

    // needed to check whether to destroy a window or not
    static inline std::atomic<bool> s_glfwInitialized = false;

    using unique_GLFWwindow = std::unique_ptr<
        GLFWwindow,
        decltype([](GLFWwindow* window) {
            if (s_glfwInitialized) {
                glfwDestroyWindow(window);
            } else {
                // do nothing, the window must have been destroyed by glfwTerminate
            }
        })>;

private:
    unique_GLFWwindow m_window{ nullptr };
    Shader            m_shader;    // uninitialized
    GLuint            m_triangleVao{};
    double            m_lastTime{};
    double            m_deltaTime{};
    bool              m_vsync{ true };
    bool              m_drawWireFrame{ false };
    bool              m_invertColor{ false };

public:
    static std::optional<std::reference_wrapper<App>> getInstance()
    {
        if (s_instance == nullptr) {
            return {};
        }
        return std::ref(*s_instance);
    }

    // returns true if the instance was created
    static bool createInstance()
    {
        if (s_instance != nullptr) {
            return true;
        }

        auto maybeWindow{ init() };
        if (!maybeWindow.has_value()) {
            std::cerr << "Error in window initialization, won't create any instance.\n";
            return false;
        } else {
            s_instance = std::unique_ptr<App>{ new App{ std::move(maybeWindow.value()) } };
            return true;
        }
    }

    static void destroyInstance()
    {
        if (s_instance != nullptr) {
            s_instance.reset();
        }

        terminate();
    }

private:
    static std::optional<unique_GLFWwindow> init()
    {
        glfwSetErrorCallback([](int errorCode, const char* description) {
            std::cerr << std::format("GLFW Error [{}]: {}\n", errorCode, description);
        });

        if (!glfwInit()) {
            std::cerr << "Failed to initialize GLFW\n";
            s_glfwInitialized = false;
            return {};
        } else {
            s_glfwInitialized = true;
        }

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        unique_GLFWwindow window{ glfwCreateWindow(s_windowWidth, s_windowHeight, s_windowName.data(), nullptr, nullptr) };
        if (window == nullptr) {
            std::cerr << "Failed to create GLFW window\n";
            glfwTerminate();
            return {};
        } else {
            glfwMakeContextCurrent(window.get());
        }

        glbinding::initialize(glfwGetProcAddress);

        glViewport(0, 0, s_windowWidth, s_windowHeight);

        return window;
    }

    static void terminate()
    {
        glfwTerminate();
        s_glfwInitialized = false;
    }

public:
    App(const App&)            = delete;
    App& operator=(const App&) = delete;
    App(App&&)                 = delete;
    App& operator=(App&&)      = delete;

    ~App()
    {
        glDeleteVertexArrays(1, &m_triangleVao);
    }

private:
    App(unique_GLFWwindow&& window)
        : m_window{ std::move(window) }
        , m_shader{ "./assets/shader/shader.vert", "./assets/shader/shader.frag" }
    {
        glfwSetWindowUserPointer(m_window.get(), this);

        glfwSetKeyCallback(m_window.get(), [](GLFWwindow* window, int key, int /* scancode */, int action, int /* mods */) {
            auto& app{ *static_cast<App*>(glfwGetWindowUserPointer(window)) };

            if (key == GLFW_KEY_W && action == GLFW_PRESS) {
                if ((app.m_drawWireFrame = !app.m_drawWireFrame)) {
                    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                } else {
                    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
                }
            }

            if (key == GLFW_KEY_I && action == GLFW_PRESS) {
                if ((app.m_invertColor = !app.m_invertColor)) {
                    app.m_shader.setUniform("u_invertColor", true);
                } else {
                    app.m_shader.setUniform("u_invertColor", false);
                }
            }

            if (key == GLFW_KEY_V && action == GLFW_PRESS) {
                if ((app.m_vsync = !app.m_vsync)) {
                    glfwSwapInterval(1);
                } else {
                    glfwSwapInterval(0);
                }
            }
        });

        glfwSetFramebufferSizeCallback(m_window.get(), [](GLFWwindow*, int width, int height) {
            glViewport(0, 0, width, height);
        });
    }

public:
    void main()
    {
        readDeviceInformation();
        prepare();
        renderLoop();
    }

private:
    void prepareVertex()
    {
        // create vertex array object
        glGenVertexArrays(1, &m_triangleVao);
        glBindVertexArray(m_triangleVao);

        // create vertex data
        GLuint vbo{};
        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(
            GL_ARRAY_BUFFER,
            sizeof(TriangleVert),
            (void*)&s_triangleVertices.front().pos.front(),
            GL_STATIC_DRAW
        );

        // positiion attrib
        glVertexAttribPointer(0, s_triangleVertices.size(), GL_FLOAT, GL_FALSE, sizeof(PosAndColor), (void*)offsetof(PosAndColor, pos));
        glEnableVertexAttribArray(0);

        // color attrib
        glVertexAttribPointer(1, s_triangleVertices.size(), GL_FLOAT, GL_FALSE, sizeof(PosAndColor), (void*)offsetof(PosAndColor, color));
        glEnableVertexAttribArray(1);

        // unbind
        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    void prepare()
    {
        prepareVertex();
    }

    void processInput(GLFWwindow* window)
    {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            glfwSetWindowShouldClose(window, true);
        }
    }

    void updateDeltaTime()
    {
        auto currentTime{ glfwGetTime() };
        m_deltaTime = currentTime - m_lastTime;
        m_lastTime  = currentTime;
    }

    void updateTitle()
    {
        constexpr double timeInterval{ 0.25 };    // in seconds
        static double    sumTime{ 0.0 };
        static int       numFrames{ 0 };

        ++numFrames;
        if ((sumTime += m_deltaTime) >= timeInterval) {
            auto avgTime{ sumTime / numFrames };
            auto newTitle{ std::format("{} [{} FPS | {:.2f}ms]", s_windowName, static_cast<int>(1 / avgTime), avgTime * 1000) };
            glfwSetWindowTitle(m_window.get(), newTitle.c_str());
            sumTime   = 0;
            numFrames = 0;
        }
    }

    void renderLoop()
    {
        auto window{ m_window.get() };

        // we only use a single shader program, set it once is okay I guess
        m_shader.use();

        while (!glfwWindowShouldClose(window)) {
            processInput(window);

            // clear buffer
            glClearColor(0.1f, 0.1f, 0.2f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);

            // draw triangle
            glBindVertexArray(m_triangleVao);
            glDrawArrays(GL_TRIANGLES, 0, s_triangleVertices.size());

            glfwSwapBuffers(window);
            glfwPollEvents();

            updateDeltaTime();
            updateTitle();
        }
    }

    void readDeviceInformation()
    {
        // device information
        auto vendor{ glGetString(GL_VENDOR) };        // Returns the vendor
        auto renderer{ glGetString(GL_RENDERER) };    // Returns a hint to the model
        std::cout << "Device: " << renderer << '\n';
        std::cout << "Vendor: " << vendor << '\n';

        int nrAttributes;
        glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &nrAttributes);
        std::cout << "Maximum number of vertex attributes supported: " << nrAttributes << '\n';
        std::cout << '\n';
    }
};

#endif /* end of include guard: APP_HPP_CCU84BHI */
