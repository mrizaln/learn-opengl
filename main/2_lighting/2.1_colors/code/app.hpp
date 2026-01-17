#ifndef APP_HPP_CCU84BHI
#define APP_HPP_CCU84BHI

#include <algorithm>
#include <array>
#include <atomic>
#include <cmath>
#include <cstddef>
#include <format>
#include <functional>
#include <glm/ext/matrix_transform.hpp>
#include <iostream>
#include <memory>
#include <optional>
#include <ranges>
#include <string_view>
#include <type_traits>
#include <unordered_map>
#include <utility>

#include <glbinding/gl/gl.h>
#include <glbinding/glbinding.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "shader.hpp"
#include "camera.hpp"

#include "util/assets_path.hpp"

using namespace gl;

class App
{
public:
    static constexpr int              DEFAULT_WINDOW_WIDTH  = 800;
    static constexpr int              DEFAULT_WINDOW_HEIGHT = 600;
    static constexpr std::string_view DEFAULT_WINDOW_NAME   = "LearnOpenGL";

    static inline auto s_assets_path = util::assets_path("2.1_colors");

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

    template <typename T>
    struct UniformData
    {
        std::string name;
        T           value;
    };

    using KeyEvent = int;    // GLFW_KEY_*
    struct KeyEventHandler
    {
        using KeyModifier = int;    // GLFW_MOD_*
        enum class KeyActionType
        {
            ONCE,
            CONTINUOUS,
        };

        KeyModifier           mods;
        KeyActionType         action;
        std::function<void()> handler;
    };
    using KeyMap = std::unordered_multimap<KeyEvent, KeyEventHandler>;

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
    // window
    unique_GLFWwindow m_window{ nullptr };
    int               m_windowWidth{ DEFAULT_WINDOW_WIDTH };
    int               m_windowHeight{ DEFAULT_WINDOW_HEIGHT };

    // mouse
    bool                m_captureMouse{ false };
    glm::vec<2, double> m_lastMousePosition{ 0.0, 0.0 };

    // camera
    Camera m_camera;

    // gl objects
    Shader m_shader;
    Shader m_lightShader;
    GLuint m_cubeVbo{};
    GLuint m_cubeVao{};
    GLuint m_lightVao{};

    // lighting
    glm::vec3 lightPos{ 1.2f, 1.0f, 2.0f };

    // timing
    double m_lastTime{};
    double m_deltaTime{};

    // key maps
    KeyMap m_keyMap;

    // options
    bool m_vsync{ true };
    bool m_drawWireFrame{ false };
    bool m_invertRender{ false };

    // uniforms

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

        unique_GLFWwindow window{ glfwCreateWindow(DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT, DEFAULT_WINDOW_NAME.data(), nullptr, nullptr) };
        if (window == nullptr) {
            std::cerr << "Failed to create GLFW window\n";
            glfwTerminate();
            return {};
        } else {
            glfwMakeContextCurrent(window.get());
        }

        glbinding::initialize(glfwGetProcAddress);

        glViewport(0, 0, DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT);

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
        glDeleteVertexArrays(1, &m_lightVao);
        glDeleteBuffers(1, &m_cubeVbo);
    }

private:
    App(unique_GLFWwindow&& window)
        : m_window{ std::move(window) }
        , m_camera({})    // use defaults
        , m_shader{
            s_assets_path / "shader/shader.vert",
            s_assets_path / "shader/shader.frag",
        }
        , m_lightShader{
            s_assets_path / "shader/shader.vert",
            s_assets_path / "shader/light_shader.frag",
        }
    {
        setCallbacks();
        setDefaultKeyEventHandler();

        // set last mouse Position (prevent sudden jump)
        glfwGetCursorPos(m_window.get(), &m_lastMousePosition.x, &m_lastMousePosition.y);

        if (m_captureMouse) {
            glfwSetInputMode(m_window.get(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        }
    }

public:
    void main()
    {
        readDeviceInformation();
        prepare();
        renderLoop();
    }

private:
    App& addKeyEventHandler(
        KeyEvent                       key,
        KeyEventHandler::KeyModifier   mods,
        KeyEventHandler::KeyActionType action,
        std::function<void()>&&        handler
    )
    {
        m_keyMap.emplace(key, KeyEventHandler{ mods, action, handler });

        return *this;
    }

    void setCallbacks()
    {
        glfwSetWindowUserPointer(m_window.get(), this);

        // this is for once key action event only (for toggling option or the like)
        glfwSetKeyCallback(m_window.get(), [](GLFWwindow* window, int key, int /* scancode */, int action, int mods) {
            auto& app{ *static_cast<App*>(glfwGetWindowUserPointer(window)) };
            auto  range{ app.m_keyMap.equal_range(key) };
            for (auto& [_, handler] : std::ranges::subrange(range.first, range.second)) {
                auto& [hmod, haction, hfun]{ handler };
                if (haction != KeyEventHandler::KeyActionType::ONCE) { continue; }
                if (action == GLFW_RELEASE || action == GLFW_REPEAT) { continue; }    // ignore release and repeat event for now

                if (mods & hmod || hmod == 0) {    // modifier match or don't have any modifier
                    hfun();
                }
            }
        });

        // mouse position callback
        glfwSetCursorPosCallback(m_window.get(), [](GLFWwindow* window, double xpos, double ypos) {
            auto& app{ *static_cast<App*>(glfwGetWindowUserPointer(window)) };

            auto& lastX{ app.m_lastMousePosition.x };
            auto& lastY{ app.m_lastMousePosition.y };

            float xoffset = static_cast<float>(xpos - lastX);
            float yoffset = static_cast<float>(lastY - ypos);
            lastX         = xpos;
            lastY         = ypos;

            if (app.m_captureMouse) {
                app.m_camera.lookAround(xoffset, yoffset);
            }
        });

        // mouse scroll callback
        glfwSetScrollCallback(m_window.get(), [](GLFWwindow* window, double /* xoffset */, double yoffset) {
            auto& app{ *static_cast<App*>(glfwGetWindowUserPointer(window)) };
            if (app.m_captureMouse) {
                app.m_camera.updatePerspective(static_cast<float>(yoffset));
            }
        });

        // framebuffer resize callback
        glfwSetFramebufferSizeCallback(m_window.get(), [](GLFWwindow* window, int width, int height) {
            glViewport(0, 0, width, height);

            auto& app{ *static_cast<App*>(glfwGetWindowUserPointer(window)) };
            app.m_windowWidth  = width;
            app.m_windowHeight = height;
        });
    }

    void setDefaultKeyEventHandler()
    {
        /// once event (toggle)
        // wireframe
        addKeyEventHandler(GLFW_KEY_W, GLFW_MOD_ALT, KeyEventHandler::KeyActionType::ONCE, [this] {
            if ((m_drawWireFrame = !m_drawWireFrame)) {
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            } else {
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            }
        });

        // vsync
        addKeyEventHandler(GLFW_KEY_V, GLFW_MOD_ALT, KeyEventHandler::KeyActionType::ONCE, [this] {
            if ((m_vsync = !m_vsync)) {
                glfwSwapInterval(1);
            } else {
                glfwSwapInterval(0);
            }
        });

        // invert render
        addKeyEventHandler(GLFW_KEY_Z, GLFW_MOD_ALT, KeyEventHandler::KeyActionType::ONCE, [this] {
            auto& value{ m_invertRender };
            if ((value = !value)) {
                glDepthFunc(GL_GREATER);
                glClearDepth(0);
            } else {
                glDepthFunc(GL_LESS);
                glClearDepth(1);
            }
        });

        // capture mouse
        addKeyEventHandler(GLFW_KEY_C, GLFW_MOD_ALT, KeyEventHandler::KeyActionType::ONCE, [this] {
            if ((m_captureMouse = !m_captureMouse)) {
                glfwSetInputMode(m_window.get(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            } else {
                glfwSetInputMode(m_window.get(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            }
        });

        // exit
        addKeyEventHandler(GLFW_KEY_Q, 0, KeyEventHandler::KeyActionType::ONCE, [this] {
            glfwSetWindowShouldClose(m_window.get(), true);
        });
        addKeyEventHandler(GLFW_KEY_ESCAPE, 0, KeyEventHandler::KeyActionType::ONCE, [this] {
            glfwSetWindowShouldClose(m_window.get(), true);
        });

        /// continuous input
        {    // camera movements (minecraft like)
            addKeyEventHandler(GLFW_KEY_W, 0, KeyEventHandler::KeyActionType::CONTINUOUS, [this] {
                m_camera.moveCamera(Camera::Movement::FORWARD, static_cast<float>(m_deltaTime));
            });
            addKeyEventHandler(GLFW_KEY_S, 0, KeyEventHandler::KeyActionType::CONTINUOUS, [this] {
                m_camera.moveCamera(Camera::Movement::BACKWARD, static_cast<float>(m_deltaTime));
            });
            addKeyEventHandler(GLFW_KEY_A, 0, KeyEventHandler::KeyActionType::CONTINUOUS, [this] {
                m_camera.moveCamera(Camera::Movement::LEFT, static_cast<float>(m_deltaTime));
            });
            addKeyEventHandler(GLFW_KEY_D, 0, KeyEventHandler::KeyActionType::CONTINUOUS, [this] {
                m_camera.moveCamera(Camera::Movement::RIGHT, static_cast<float>(m_deltaTime));
            });
            addKeyEventHandler(GLFW_KEY_LEFT_SHIFT, 0, KeyEventHandler::KeyActionType::CONTINUOUS, [this] {
                m_camera.moveCamera(Camera::Movement::DOWNWARD, static_cast<float>(m_deltaTime));
            });
            addKeyEventHandler(GLFW_KEY_SPACE, 0, KeyEventHandler::KeyActionType::CONTINUOUS, [this] {
                m_camera.moveCamera(Camera::Movement::UPWARD, static_cast<float>(m_deltaTime));
            });
        }
    }

    void prepareObject()
    {
        // ------------[ create cube vertex ]--------------
        // create vertex array object
        glGenVertexArrays(1, &m_cubeVao);
        glBindVertexArray(m_cubeVao);

        // create vertex data
        glGenBuffers(1, &m_cubeVbo);
        glBindBuffer(GL_ARRAY_BUFFER, m_cubeVbo);
        glBufferData(
            GL_ARRAY_BUFFER,
            sizeof(s_cubeVertices),
            (void*)&s_cubeVertices.front().pos.front(),
            GL_STATIC_DRAW
        );

        // positiion attrib
        glVertexAttribPointer(0, VertexData{}.pos.size(), GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)offsetof(VertexData, pos));
        glEnableVertexAttribArray(0);

        // unbind
        glBindVertexArray(0);
        // ------------------------------------------------

        // -----------[ create light vertex using the cube vertex ]-------------
        // create vertex array object
        glGenVertexArrays(1, &m_lightVao);
        glBindVertexArray(m_lightVao);

        // we only need to bind to the VBO, the cube VBO's data already contains the data
        glBindBuffer(GL_ARRAY_BUFFER, m_cubeVbo);

        // positiion attrib
        glVertexAttribPointer(0, VertexData{}.pos.size(), GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)offsetof(VertexData, pos));
        glEnableVertexAttribArray(0);

        // unbind
        glBindVertexArray(0);
        // ---------------------------------------------------------------------

        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    // TODO: implement this!
    void prepareLight()
    {
    }

    void prepare()
    {
        prepareObject();
        prepareLight();
    }

    void processInput(GLFWwindow* window)
    {
        const auto getMods = [win = window] {
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

        for (auto& [key, handler] : m_keyMap) {
            auto& [hmod, haction, hfun]{ handler };
            if (haction != KeyEventHandler::KeyActionType::CONTINUOUS) { continue; }
            if (glfwGetKey(window, key) != GLFW_PRESS) { continue; }

            auto mods{ getMods() };
            if (mods & hmod || hmod == 0) {    // modifier match or don't have any modifier
                hfun();
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
            auto newTitle{ std::format("{} [{} FPS | {:.2f}ms]", DEFAULT_WINDOW_NAME, static_cast<int>(1 / avgTime), avgTime * 1000) };
            glfwSetWindowTitle(m_window.get(), newTitle.c_str());
            sumTime   = 0;
            numFrames = 0;
        }
    }

    void renderLoop()
    {
        auto window{ m_window.get() };

        m_shader.use();
        m_shader.setUniform("u_objectColor", 1.0f, 0.5f, 0.31f);
        m_shader.setUniform("u_lightColor", 1.0f, 1.0f, 1.0f);

        while (!glfwWindowShouldClose(window)) {
            processInput(window);

            // clear buffer
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            auto      view{ m_camera.getViewMatrix() };
            auto      projection{ m_camera.getProjectionMatrix(m_windowWidth, m_windowHeight) };
            glm::mat4 model{ 1.0f };

            //----------------[ cube container object ]-----------------
            m_shader.use();
            m_shader.setUniform("u_view", view);
            m_shader.setUniform("u_projection", projection);
            m_shader.setUniform("u_model", model);

            glBindVertexArray(m_cubeVao);
            glDrawArrays(GL_TRIANGLES, 0, s_cubeVertices.size());
            //----------------------------------------------------------
            //----------------[ light cube object ]-----------------
            m_lightShader.use();
            m_lightShader.setUniform("u_view", view);
            m_lightShader.setUniform("u_projection", projection);

            model = glm::translate(glm::mat4{ 1.0f }, lightPos);
            model = glm::scale(model, glm::vec3(0.2f));
            m_shader.setUniform("u_model", model);

            glBindVertexArray(m_lightVao);
            glDrawArrays(GL_TRIANGLES, 0, s_cubeVertices.size());
            //------------------------------------------------------

            glfwSwapBuffers(window);
            glfwPollEvents();

            updateDeltaTime();
            updateTitle();
        }
    }

    void
    readDeviceInformation()
    {
        // device information
        auto vendor{ glGetString(GL_VENDOR) };        // Returns the vendor
        auto renderer{ glGetString(GL_RENDERER) };    // Returns a hint to the model
        std::cout << "Device: " << renderer << '\n';
        std::cout << "Vendor: " << vendor << '\n';

        // int nrAttributes;
        // glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &nrAttributes);
        // std::cout << "Maximum number of vertex attributes supported: " << nrAttributes << '\n';

        std::cout << '\n';
    }
};

#endif /* end of include guard: APP_HPP_CCU84BHI */
