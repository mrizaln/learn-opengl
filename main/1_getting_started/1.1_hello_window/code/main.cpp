#include <atomic>
#include <format>
#include <iostream>
#include <format>
#include <memory>
#include <optional>

#include <glad/glad.h>    // make sure to
#include <GLFW/glfw3.h>

constexpr int         g_window_width  = 800;
constexpr int         g_window_height = 600;
constexpr std::string g_window_name   = "LearnOpenGL";

// needed to check whether to destroy a window or not
static std::atomic<bool> g_glfw_initialized = false;

using unique_GLFWwindow = std::unique_ptr<
    GLFWwindow,
    decltype([](GLFWwindow* window) {
        if (g_glfw_initialized) {
            glfwDestroyWindow(window);
            std::cout << "destroy\n";
        } else {
            // do nothing, the window must have been destroyed by glfwTerminate
        }
    })>;

std::optional<unique_GLFWwindow> init()
{
    // set error callback
    glfwSetErrorCallback([](int errorCode, const char* description) {
        std::cerr << std::format("GLFW Error [{}]: {}\n", errorCode, description);
    });

    // initialize glfw
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW\n";
        g_glfw_initialized = false;
        return {};
    } else {
        g_glfw_initialized = true;
    }

    // set opengl version and profile
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // create glfw window and its associated opengl context
    unique_GLFWwindow window{ glfwCreateWindow(g_window_width, g_window_height, g_window_name.c_str(), nullptr, nullptr) };
    if (window == nullptr) {
        std::cerr << "Failed to create GLFW window\n";
        glfwTerminate();
        return {};
    } else {
        glfwMakeContextCurrent(window.get());
    }

    // initialize glad
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize glad";
        return {};
    }

    // set the size of rendering window
    glViewport(0, 0, g_window_width, g_window_height);

    // set framebuffer resize callback
    glfwSetFramebufferSizeCallback(window.get(), [](GLFWwindow*, int width, int height) {
        glViewport(0, 0, width, height);
    });

    return window;
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
}

void renderLoop(GLFWwindow* window)
{
    while (!glfwWindowShouldClose(window)) {
        processInput(window);

        glClearColor(0.1f, 0.1f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}

void terminate()
{
    glfwTerminate();
    g_glfw_initialized = false;
}

int main()
{
    auto maybeWindow{ init() };
    if (!maybeWindow.has_value()) {
        std::cerr << "Error in initialization, terminating...\n";
        return 1;
    }
    unique_GLFWwindow window{ std::move(maybeWindow.value()) };

    renderLoop(window.get());

    terminate();
}
