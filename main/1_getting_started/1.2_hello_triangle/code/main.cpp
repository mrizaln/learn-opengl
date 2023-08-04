#include <array>
#include <atomic>
#include <format>
#include <format>
#include <iostream>
#include <memory>
#include <optional>
#include <string_view>

#include <glad/glad.h>    // make sure to
#include <GLFW/glfw3.h>

constexpr int              g_widowWidth   = 800;
constexpr int              g_windowHeight = 600;
constexpr std::string_view g_windowName   = "LearnOpenGL";

using Point    = std::array<float, 3>;
using Triangle = std::array<Point, 3>;

// needed to check whether to destroy a window or not
static std::atomic<bool> g_glfwInitialized = false;

constexpr Triangle g_vertices{
    { { -0.5f, -0.5f, 0.0f },
      { 0.5f, -0.5f, 0.0f },
      { 0.0f, 0.5f, 0.0f } }
};

constexpr std::string_view g_vertexShaderSource = R"glsl(
    #version 330 core
    layout (location = 0) in vec3 aPos;

    void main()
    {
        gl_Position = vec4(aPos, 1.0);
    }
)glsl";

constexpr std::string_view g_fragmentShaderSource = R"glsl(
    #version 330 core
    out vec4 FragColor;

    void main()
    {
        FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);
    }
)glsl";

using unique_GLFWwindow = std::unique_ptr<
    GLFWwindow,
    decltype([](GLFWwindow* window) {
        if (g_glfwInitialized) {
            glfwDestroyWindow(window);
            std::cout << "destroy\n";
        } else {
            // do nothing, the window must have been destroyed by glfwTerminate
        }
    })>;

namespace app
{
    unique_GLFWwindow window{ nullptr };
    GLuint            triangleVao{};
    GLuint            shaderProgram{};
}

std::optional<unique_GLFWwindow> init()
{
    glfwSetErrorCallback([](int errorCode, const char* description) {
        std::cerr << std::format("GLFW Error [{}]: {}\n", errorCode, description);
    });

    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW\n";
        g_glfwInitialized = false;
        return {};
    } else {
        g_glfwInitialized = true;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    unique_GLFWwindow window{ glfwCreateWindow(g_widowWidth, g_windowHeight, g_windowName.data(), nullptr, nullptr) };
    if (window == nullptr) {
        std::cerr << "Failed to create GLFW window\n";
        glfwTerminate();
        return {};
    } else {
        glfwMakeContextCurrent(window.get());
    }

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize glad";
        return {};
    }

    glViewport(0, 0, g_widowWidth, g_windowHeight);

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

void renderLoop()
{
    auto window{ app::window.get() };

    // glfwSwapInterval(0);
    // double time{ 0.0 };

    while (!glfwWindowShouldClose(window)) {
        processInput(window);

        // clear buffer
        glClearColor(0.1f, 0.1f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // draw
        glUseProgram(app::shaderProgram);
        glBindVertexArray(app::triangleVao);
        glDrawArrays(GL_TRIANGLES, 0, g_vertices.size());

        glfwSwapBuffers(window);
        glfwPollEvents();

        // auto prevTime{ time };
        // time = glfwGetTime();
        // std::cout << "FPS: " << 1 / (time - prevTime) << '\n';
    }
}

void terminate()
{
    glfwTerminate();
    g_glfwInitialized = false;
}

void shaderCompileInfo(GLuint shader)
{
    GLint status{};
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if (status != 1) {
        GLint maxLength{};
        GLint logLength{};

        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);
        auto log{ new GLchar[(std::size_t)maxLength] };
        glGetShaderInfoLog(shader, maxLength, &logLength, log);
        std::cout << "Shader compilation failed: \n"
                  << log << '\n';
        delete[] log;
    }
}

void shaderLinkInfo(GLuint program)
{
    GLint status{};
    glGetProgramiv(program, GL_LINK_STATUS, &status);
    if (status != 1) {
        GLint maxLength{};
        GLint logLength{};

        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);
        auto log{ new GLchar[(std::size_t)maxLength] };
        glGetProgramInfoLog(program, maxLength, &logLength, log);
        std::cout << "Program linking failed: \n"
                  << log << '\n';
        delete[] log;
    }
}

void prepareVertex()
{
    // create vertex array object
    GLuint vao{};
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    app::triangleVao = vao;

    // create vertex data
    GLuint vbo{};
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Triangle), (void*)&g_vertices.front().front(), GL_STATIC_DRAW);

    // configure vertex attributes
    glVertexAttribPointer(
        0,
        g_vertices.size(),
        GL_FLOAT,
        GL_FALSE,
        sizeof(decltype(g_vertices)::value_type),
        (void*)0
    );
    glEnableVertexAttribArray(0);

    // unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void prepareShader()
{
    // compile vertex shader
    GLuint      vertexShader{ glCreateShader(GL_VERTEX_SHADER) };
    const char* vertexShaderSource{ g_vertexShaderSource.data() };
    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);    // it expects an array of const char* (sigh, I don't like C...)
    glCompileShader(vertexShader);
    shaderCompileInfo(vertexShader);

    // compile fragment shader
    GLuint      fragmentShader{ glCreateShader(GL_FRAGMENT_SHADER) };
    const char* fragmentShaderSource{ g_fragmentShaderSource.data() };
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);    // it expects an array of const char* (sigh, I don't like C...)
    glCompileShader(fragmentShader);
    shaderCompileInfo(fragmentShader);

    // create shader program then link shaders to it
    GLuint shaderProgram{ glCreateProgram() };
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    app::shaderProgram = shaderProgram;

    // delete shader objects
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

void prepare()
{
    prepareVertex();
    prepareShader();
}

int main()
{
    auto maybeWindow{ init() };
    if (!maybeWindow.has_value()) {
        std::cerr << "Error in initialization, terminating...\n";
        return 1;
    }
    app::window = std::move(maybeWindow.value());
    maybeWindow.reset();

    prepare();    // most things happenned here

    renderLoop();

    terminate();
}
