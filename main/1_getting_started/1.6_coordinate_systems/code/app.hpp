#ifndef APP_HPP_CCU84BHI
#define APP_HPP_CCU84BHI

#include <array>
#include <atomic>
#include <cmath>
#include <cstddef>
#include <format>
#include <functional>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/geometric.hpp>
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
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "shader.hpp"

using namespace gl;

class App
{
public:
    static constexpr int              s_windowWidth  = 800;
    static constexpr int              s_windowHeight = 600;
    static constexpr std::string_view s_windowName   = "LearnOpenGL";

    template <typename T>
    using Pair = std::array<T, 2>;

    template <typename T>
    using Triple = std::array<T, 3>;

    template <typename T = float>
    using Point = Triple<T>;

    using VertexData = struct
    {
        Point<float> pos;
        Pair<float>  texCoord;
    };

    // clang-format off
    static constexpr std::array<VertexData, 36> s_cubeVertices{ {
        { .pos = { -0.5f, -0.5f, -0.5f }, .texCoord = { 0.0f, 0.0f } },
        { .pos = {  0.5f, -0.5f, -0.5f }, .texCoord = { 1.0f, 0.0f } },
        { .pos = {  0.5f,  0.5f, -0.5f }, .texCoord = { 1.0f, 1.0f } },
        { .pos = {  0.5f,  0.5f, -0.5f }, .texCoord = { 1.0f, 1.0f } },
        { .pos = { -0.5f,  0.5f, -0.5f }, .texCoord = { 0.0f, 1.0f } },
        { .pos = { -0.5f, -0.5f, -0.5f }, .texCoord = { 0.0f, 0.0f } },

        { .pos = { -0.5f, -0.5f,  0.5f }, .texCoord = { 0.0f, 0.0f } },
        { .pos = {  0.5f, -0.5f,  0.5f }, .texCoord = { 1.0f, 0.0f } },
        { .pos = {  0.5f,  0.5f,  0.5f }, .texCoord = { 1.0f, 1.0f } },
        { .pos = {  0.5f,  0.5f,  0.5f }, .texCoord = { 1.0f, 1.0f } },
        { .pos = { -0.5f,  0.5f,  0.5f }, .texCoord = { 0.0f, 1.0f } },
        { .pos = { -0.5f, -0.5f,  0.5f }, .texCoord = { 0.0f, 0.0f } },

        { .pos = { -0.5f,  0.5f,  0.5f }, .texCoord = { 1.0f, 0.0f } },
        { .pos = { -0.5f,  0.5f, -0.5f }, .texCoord = { 1.0f, 1.0f } },
        { .pos = { -0.5f, -0.5f, -0.5f }, .texCoord = { 0.0f, 1.0f } },
        { .pos = { -0.5f, -0.5f, -0.5f }, .texCoord = { 0.0f, 1.0f } },
        { .pos = { -0.5f, -0.5f,  0.5f }, .texCoord = { 0.0f, 0.0f } },
        { .pos = { -0.5f,  0.5f,  0.5f }, .texCoord = { 1.0f, 0.0f } },

        { .pos = {  0.5f,  0.5f,  0.5f }, .texCoord = { 1.0f, 0.0f } },
        { .pos = {  0.5f,  0.5f, -0.5f }, .texCoord = { 1.0f, 1.0f } },
        { .pos = {  0.5f, -0.5f, -0.5f }, .texCoord = { 0.0f, 1.0f } },
        { .pos = {  0.5f, -0.5f, -0.5f }, .texCoord = { 0.0f, 1.0f } },
        { .pos = {  0.5f, -0.5f,  0.5f }, .texCoord = { 0.0f, 0.0f } },
        { .pos = {  0.5f,  0.5f,  0.5f }, .texCoord = { 1.0f, 0.0f } },

        { .pos = { -0.5f, -0.5f, -0.5f }, .texCoord = { 0.0f, 1.0f } },
        { .pos = {  0.5f, -0.5f, -0.5f }, .texCoord = { 1.0f, 1.0f } },
        { .pos = {  0.5f, -0.5f,  0.5f }, .texCoord = { 1.0f, 0.0f } },
        { .pos = {  0.5f, -0.5f,  0.5f }, .texCoord = { 1.0f, 0.0f } },
        { .pos = { -0.5f, -0.5f,  0.5f }, .texCoord = { 0.0f, 0.0f } },
        { .pos = { -0.5f, -0.5f, -0.5f }, .texCoord = { 0.0f, 1.0f } },

        { .pos = { -0.5f,  0.5f, -0.5f }, .texCoord = { 0.0f, 1.0f } },
        { .pos = {  0.5f,  0.5f, -0.5f }, .texCoord = { 1.0f, 1.0f } },
        { .pos = {  0.5f,  0.5f,  0.5f }, .texCoord = { 1.0f, 0.0f } },
        { .pos = {  0.5f,  0.5f,  0.5f }, .texCoord = { 1.0f, 0.0f } },
        { .pos = { -0.5f,  0.5f,  0.5f }, .texCoord = { 0.0f, 0.0f } },
        { .pos = { -0.5f,  0.5f, -0.5f }, .texCoord = { 0.0f, 1.0f } },
    } };
    // clang-format on

    static constexpr std::array<glm::vec3, 10> s_cubePositions{ {
        // clang-format off
        {  0.0f,  0.0f,  0.0f },
        {  2.0f,  5.0f, -15.0f},
        { -1.5f, -2.2f, -2.5f },
        { -3.8f, -2.0f, -12.3f},
        {  2.4f, -0.4f, -3.5f },
        { -1.7f,  3.0f, -7.5f },
        {  1.3f, -2.0f, -2.5f },
        {  1.5f,  2.0f, -2.5f },
        {  1.5f,  0.2f, -1.5f },
        { -1.3f,  1.0f, -1.5f },
        // clang-format on
    } };

    class ImageData
    {
    public:
        const int                  m_width{};
        const int                  m_height{};
        const int                  m_nrChannels{};
        const unsigned char* const m_data{};

    public:
        ImageData(const ImageData&)            = delete;
        ImageData& operator=(const ImageData&) = delete;

        ~ImageData()
        {
            stbi_image_free(const_cast<unsigned char*>(m_data));
        }

        // private:
        // don't use this directly, use from() instead
        ImageData(int w, int h, int c, unsigned char* d)
            : m_width{ w }
            , m_height{ h }
            , m_nrChannels{ c }
            , m_data{ d }
        {
        }

    public:
        static std::optional<ImageData> from(std::filesystem::path imagePath)
        {
            int            width, height, nrChannels;
            unsigned char* data{ stbi_load(imagePath.c_str(), &width, &height, &nrChannels, 0) };
            if (!data) {
                std::cerr << std::format("Failed to load image at {}\n", imagePath.string());
                return {};
            }
            return std::make_optional<ImageData>(width, height, nrChannels, data);
        }
    };

    struct TextureData
    {
        GLuint                id;
        std::filesystem::path texturePath;
    };

    template <typename T>
    struct UniformData
    {
        std::string name;
        T           value;
    };

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
    int               m_windowWidth{ s_windowWidth };
    int               m_windowHeight{ s_windowHeight };
    Shader            m_shader;
    GLuint            m_cubeVao{};
    GLuint            m_texture0;
    GLuint            m_texture1;
    double            m_lastTime{};
    double            m_deltaTime{};
    bool              m_vsync{ true };
    bool              m_drawWireFrame{ false };
    bool              m_invertRender{ false };

    UniformData<float> u_mixValue{ "u_mixValue", 0.4f };
    UniformData<bool>  u_invertColor{ "u_invertColor", false };

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

        // enable depth test
        glEnable(GL_DEPTH_TEST);

        // set clear color
        glClearColor(0.1f, 0.1f, 0.2f, 1.0f);

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
        glDeleteVertexArrays(1, &m_cubeVao);
    }

private:
    App(unique_GLFWwindow&& window)
        : m_window{ std::move(window) }
        , m_shader{
            "./assets/shader/shader.vert",
            "./assets/shader/shader.frag",
        }
    {
        glfwSetWindowUserPointer(m_window.get(), this);

        glfwSetKeyCallback(m_window.get(), [](GLFWwindow* window, int key, int /* scancode */, int action, int /* mods */) {
            auto& app{ *static_cast<App*>(glfwGetWindowUserPointer(window)) };

            // wireframe
            if (key == GLFW_KEY_W && action == GLFW_PRESS) {
                if ((app.m_drawWireFrame = !app.m_drawWireFrame)) {
                    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                } else {
                    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
                }
            }

            // vsync
            if (key == GLFW_KEY_V && action == GLFW_PRESS) {
                if ((app.m_vsync = !app.m_vsync)) {
                    glfwSwapInterval(1);
                } else {
                    glfwSwapInterval(0);
                }
            }

            // invert color
            if (key == GLFW_KEY_I && action == GLFW_PRESS) {
                auto& [name, value]{ app.u_invertColor };
                if ((value = !value)) {
                    app.m_shader.setUniform(name, true);
                } else {
                    app.m_shader.setUniform(name, false);
                }
            }

            // invert render
            if (key == GLFW_KEY_Z && action == GLFW_PRESS) {
                auto& value{ app.m_invertRender };
                if ((value = !value)) {
                    glDepthFunc(GL_GREATER);
                    glClearDepth(0);
                } else {
                    glDepthFunc(GL_LESS);
                    glClearDepth(1);
                }
            }

            // exit
            if ((key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) || (key == GLFW_KEY_Q && action == GLFW_PRESS)) {
                glfwSetWindowShouldClose(window, true);
            }
        });

        glfwSetFramebufferSizeCallback(m_window.get(), [](GLFWwindow* window, int width, int height) {
            glViewport(0, 0, width, height);

            auto& app{ *static_cast<App*>(glfwGetWindowUserPointer(window)) };
            app.m_windowWidth  = width;
            app.m_windowHeight = height;
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
        glGenVertexArrays(1, &m_cubeVao);
        glBindVertexArray(m_cubeVao);

        // create vertex data
        GLuint vbo{};
        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(
            GL_ARRAY_BUFFER,
            sizeof(s_cubeVertices),
            (void*)&s_cubeVertices.front().pos.front(),
            GL_STATIC_DRAW
        );

        // positiion attrib
        glVertexAttribPointer(0, VertexData{}.pos.size(), GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)offsetof(VertexData, pos));
        glEnableVertexAttribArray(0);

        // texture attrib
        glVertexAttribPointer(1, VertexData{}.texCoord.size(), GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)offsetof(VertexData, texCoord));
        glEnableVertexAttribArray(1);

        // unbind
        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    void prepareTexture()
    {
        stbi_set_flip_vertically_on_load(true);

        auto imageData{ ImageData::from("./assets/texture/container.jpg") };
        if (!imageData.has_value()) {
            std::cerr << "Failed to load image data\n";
        }

        glGenTextures(1, &m_texture0);
        glBindTexture(GL_TEXTURE_2D, m_texture0);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        GLenum format;
        if (imageData->m_nrChannels == 3) {
            format = GL_RGB;
        } else if (imageData->m_nrChannels == 4) {
            format = GL_RGBA;
        } else {
            std::cerr << "Image number of channels is not supported: " << imageData->m_nrChannels << '\n';
            format = GL_RGB;    // fallback
        }
        glTexImage2D(GL_TEXTURE_2D, 0, format, imageData->m_width, imageData->m_height, 0, format, GL_UNSIGNED_BYTE, imageData->m_data);
        glGenerateMipmap(GL_TEXTURE_2D);

        auto imageData2{ ImageData::from("./assets/texture/awesomeface.png") };
        if (!imageData2.has_value()) {
            std::cerr << "Failed to load image data\n";
        }

        glGenTextures(1, &m_texture1);
        glBindTexture(GL_TEXTURE_2D, m_texture1);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        if (imageData2->m_nrChannels == 3) {
            format = GL_RGB;
        } else if (imageData2->m_nrChannels == 4) {
            format = GL_RGBA;
        } else {
            std::cerr << "Image number of channels is not supported: " << imageData2->m_nrChannels << '\n';
            format = GL_RGB;    // fallback
        }
        glTexImage2D(GL_TEXTURE_2D, 0, format, imageData2->m_width, imageData2->m_height, 0, format, GL_UNSIGNED_BYTE, imageData2->m_data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }

    void prepare()
    {
        prepareVertex();
        prepareTexture();
    }

    void processInput(GLFWwindow* window)
    {
        {    // color mix
            auto& [name, value]{ u_mixValue };
            float multiplier{ 1.0f };
            if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS) {
                value = 0.0f;
                m_shader.setUniform(name, value);
            } else if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS) {
                value = 1.0f;
                m_shader.setUniform(name, value);
            } else if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS) {
                value = std::clamp(value - 0.01f * multiplier, 0.0f, 1.0f);
                m_shader.setUniform(name, value);
            } else if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS) {
                value = std::clamp(value + 0.01f * multiplier, 0.0f, 1.0f);
                m_shader.setUniform(name, value);
            }
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

        m_shader.use();
        m_shader.setUniform(u_mixValue.name, u_mixValue.value);
        m_shader.setUniform("u_texture0", 0);
        m_shader.setUniform("u_texture1", 1);

        // bind textures
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_texture0);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, m_texture1);

        // set once because we only have one object to draw
        glBindVertexArray(m_cubeVao);

        while (!glfwWindowShouldClose(window)) {
            processInput(window);

            // clear buffer
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            // transformation stuff
            auto view{ glm::translate(glm::mat4{ 1.0f }, { 0.0f, 0.0f, -3.0f }) };
            m_shader.setUniform("view", view);
            auto projection{ glm::perspective(glm::radians(45.0f), m_windowWidth / (float)m_windowHeight, 0.1f, 100.0f) };
            m_shader.setUniform("projection", projection);

            for (int i{ 0 }; const auto& pos : s_cubePositions) {
                auto      offset{ i++ };
                glm::vec3 rotationAxis{
                    std::sin(glfwGetTime() * (2 + offset % 3) + 60 * offset),
                    std::cos(glfwGetTime() / (100 * (1 + offset % 3))),
                    std::atan(glfwGetTime())
                };
                auto model{ glm::translate(glm::mat4{ 1.0f }, pos) };
                model = glm::rotate(model, (float)glfwGetTime(), glm::normalize(rotationAxis));
                m_shader.setUniform("model", model);

                // draw cube
                glDrawArrays(GL_TRIANGLES, 0, s_cubeVertices.size());
            }

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
