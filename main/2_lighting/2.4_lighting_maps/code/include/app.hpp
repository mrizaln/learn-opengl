#include <array>
#include <atomic>
#include <cmath>
#include <format>
#include <iostream>
#include <optional>
#include <thread>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "glbinding/gl/gl.h"

#include "window.hpp"
#include "window_manager.hpp"
#include "cube.hpp"
#include "camera.hpp"
#include "shader.hpp"
#include "texture.hpp"

template <typename T>
struct UniformData
{
    std::string m_name;
    T           m_value;
};

struct MaterialUniform
{
    std::string m_name;
    Texture     m_diffuse;
    Texture     m_specular;
    Texture     m_emission;
    float       m_shininess;

    MaterialUniform(
        const std::string&    name,
        std::filesystem::path diffuseMap,
        std::filesystem::path specularMap,
        std::filesystem::path emissionMap,
        float                 shininess
    )
        : m_name{ name }
        , m_diffuse{ Texture::from(diffuseMap, m_name + ".m_diffuse", 0).value() }       // unwrap
        , m_specular{ Texture::from(specularMap, m_name + ".m_specular", 1).value() }    // unwrap
        , m_emission{ Texture::from(emissionMap, m_name + ".m_emission", 2).value() }    // unwrap
        , m_shininess{ shininess }
    {
    }

    void applyUniform(Shader& shader) const
    {
        m_diffuse.activate(shader);
        m_specular.activate(shader);
        m_emission.activate(shader);
        shader.setUniform(m_name + ".m_shininess", m_shininess);
    }
};

struct LightUniform
{
    std::string m_name;
    glm::vec3   m_position;
    glm::vec3   m_ambient;
    glm::vec3   m_diffuse;
    glm::vec3   m_specular;

    void applyUniforms(Shader& shader) const
    {
        shader.setUniform(m_name + ".m_position", m_position);
        shader.setUniform(m_name + ".m_ambient", m_ambient);
        shader.setUniform(m_name + ".m_diffuse", m_diffuse);
        shader.setUniform(m_name + ".m_specular", m_specular);
    }
};

class App
{
private:
    static inline std::unique_ptr<App> s_instance{ nullptr };

    window::Window& m_window;
    Camera          m_camera;
    Shader          m_shader;
    Shader          m_lightShader;
    Cube            m_cube;
    Cube            m_lightCube;
    MaterialUniform m_material;
    LightUniform    m_light;

    // options
    bool m_vsync{ true };
    bool m_drawWireFrame{ false };
    bool m_invertRender{ false };
    bool m_rotate{ false };
    bool m_lightFollowCamera{ false };
    bool m_enableEmissionMap{ false };

public:
    static std::optional<std::reference_wrapper<App>> getInstance()
    {
        if (s_instance == nullptr) {
            return {};
        }
        return std::ref(*s_instance);
    }

    // returns true if the instance was created
    static void createInstance(window::Window& window)
    {
        s_instance.reset(new App{ window });
    }

    static void destroyInstance()
    {
        if (s_instance != nullptr) {
            s_instance.reset();
        }
    }

public:
    App(const App&)            = delete;
    App& operator=(const App&) = delete;
    App(App&&)                 = delete;
    App& operator=(App&&)      = delete;

private:
    App(window::Window& window)
        : m_window{ window }
        , m_camera{ {} }
        , m_shader{
            "./assets/shader/shader.vert",
            "./assets/shader/shader.frag",
        }
        , m_lightShader{
            "./assets/shader/shader.vert",
            "./assets/shader/light_shader.frag",
        }
        , m_cube{}
        , m_material{
            /* .m_name      = */ "u_material",
            /* .m_diffuse   = */ "./assets/texture/container2.png",
            /* .m_specular  = */ "./assets/texture/container2_specular.png",
            /* .m_emission  = */ "./assets/texture/abyss.jpg",
            /* .m_shininess = */ 32.0f,
        }
        , m_light{
            .m_name     = "u_light",
            .m_position = { 1.2f, 1.0f, 2.0f },
            .m_ambient  = { 0.2f, 0.2f, 0.2f },
            .m_diffuse  = { 0.5f, 0.5f, 0.5f },
            .m_specular = { 1.0f, 1.0f, 1.0f },
        }
    {
        setWindowEventsHandler();
    }

public:
    void readDeviceInformation()
    {
        // device information
        auto vendor{ glGetString(gl::GL_VENDOR) };        // Returns the vendor
        auto renderer{ glGetString(gl::GL_RENDERER) };    // Returns a hint to the model
        std::cout << "Device: " << renderer << '\n';
        std::cout << "Vendor: " << vendor << '\n';

        // int nrAttributes;
        // glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &nrAttributes);
        // std::cout << "Maximum number of vertex attributes supported: " << nrAttributes << '\n';

        std::cout << '\n';
    }

    void init()
    {
        gl::glClearColor(0.1f, 0.1f, 0.2f, 1.0f);
        gl::glEnable(gl::GL_DEPTH_TEST);

        m_shader.use();
        m_material.applyUniform(m_shader);
        m_light.applyUniforms(m_shader);
    }

    void render()
    {
        // clear buffers and update viewport
        gl::glClear(gl::GL_COLOR_BUFFER_BIT | gl::GL_DEPTH_BUFFER_BIT);
        const auto& winProp{ m_window.getProperties() };
        gl::glViewport(0, 0, winProp.m_width, winProp.m_height);
        auto      view{ m_camera.getViewMatrix() };
        auto      projection{ m_camera.getProjectionMatrix(m_window.getProperties().m_width, m_window.getProperties().m_height) };
        glm::mat4 model{ 1.0f };

        if (m_lightFollowCamera) {
            m_shader.use();
            auto light{ m_light };                     // copy
            light.m_position = m_camera.m_position;    // the light follows the camera
            light.applyUniforms(m_shader);
            m_material.applyUniform(m_shader);
        } else {
            m_shader.use();
            m_light.applyUniforms(m_shader);
            m_material.applyUniform(m_shader);

            //----------------[ light cube object ]-----------------
            m_lightShader.use();
            m_lightShader.setUniform("u_view", view);
            m_lightShader.setUniform("u_projection", projection);

            model = glm::mat4{ 1.0f };
            model = glm::translate(glm::mat4{ 1.0f }, m_light.m_position);
            model = glm::scale(model, glm::vec3(0.2f));
            m_lightShader.setUniform("u_model", model);

            m_lightCube.draw();
            //------------------------------------------------------
        }

        //----------------[ cube container object ]-----------------
        m_shader.use();
        m_shader.setUniform("u_view", view);
        m_shader.setUniform("u_projection", projection);

        static double lastTime{ 0.0 };    // yeah, this local static variable is not good
        if (m_rotate) { lastTime += m_window.getDeltaTime(); }
        glm::vec3 rotationAxis{
            std::sin(lastTime * (2 % 3) + 60),
            std::cos(lastTime / (100 * (1 % 3))),
            std::atan(lastTime)
        };
        model = glm::mat4{ 1.0f };
        model = glm::rotate(model, (float)lastTime, glm::normalize(rotationAxis));
        m_shader.setUniform("u_model", model);
        m_shader.setUniform("u_viewPos", m_camera.m_position);

        m_cube.draw();
        //----------------------------------------------------------
    }

private:
    void setWindowEventsHandler()
    {
        using enum window::Window::KeyActionType;

        m_window
            // wireframe
            .addKeyEventHandler(GLFW_KEY_W, GLFW_MOD_ALT, CALLBACK, [this](window::Window& /* win */) {
                if ((m_drawWireFrame = !m_drawWireFrame)) {
                    glPolygonMode(gl::GL_FRONT_AND_BACK, gl::GL_LINE);
                } else {
                    glPolygonMode(gl::GL_FRONT_AND_BACK, gl::GL_FILL);
                }
            })
            // vsync
            .addKeyEventHandler(GLFW_KEY_V, GLFW_MOD_ALT, CALLBACK, [this](window::Window& /* win */) {
                if ((m_vsync = !m_vsync)) {
                    glfwSwapInterval(1);
                } else {
                    glfwSwapInterval(0);
                }
            })
            // invert render
            .addKeyEventHandler(GLFW_KEY_Z, GLFW_MOD_ALT, CALLBACK, [this](window::Window& /* win */) {
                auto& value{ m_invertRender };
                if ((value = !value)) {
                    glDepthFunc(gl::GL_GREATER);
                    gl::glClearDepth(0);
                } else {
                    glDepthFunc(gl::GL_LESS);
                    gl::glClearDepth(1);
                }
            })
            // rotate
            .addKeyEventHandler(GLFW_KEY_R, GLFW_MOD_ALT, CALLBACK, [this](window::Window& /* win */) {
                m_rotate = !m_rotate;
            })
            // light follow camera
            .addKeyEventHandler(GLFW_KEY_L, GLFW_MOD_ALT, CALLBACK, [this](window::Window& /* win */) {
                m_lightFollowCamera = !m_lightFollowCamera;
            })
            // enable emission map
            .addKeyEventHandler(GLFW_KEY_E, GLFW_MOD_ALT, CALLBACK, [this](window::Window& /* win */) {
                m_shader.setUniform("u_enableEmissionMap", (m_enableEmissionMap = !m_enableEmissionMap));
            })
            // capture mouse
            .addKeyEventHandler(GLFW_KEY_C, GLFW_MOD_ALT, CALLBACK, [](window::Window& win) {
                win.setCaptureMouse(!win.isMouseCaptured());
            })
            // exit
            .addKeyEventHandler({ GLFW_KEY_Q, GLFW_KEY_ESCAPE }, 0, CALLBACK, [](window::Window& win) {
                win.requestClose();
            })

            // camera movements (minecraft-like)
            .addKeyEventHandler(GLFW_KEY_W, 0, CONTINUOUS, [this](window::Window& win) {
                m_camera.moveCamera(Camera::Movement::FORWARD, static_cast<float>(win.getDeltaTime()));
            })
            .addKeyEventHandler(GLFW_KEY_S, 0, CONTINUOUS, [this](window::Window& win) {
                m_camera.moveCamera(Camera::Movement::BACKWARD, static_cast<float>(win.getDeltaTime()));
            })
            .addKeyEventHandler(GLFW_KEY_A, 0, CONTINUOUS, [this](window::Window& win) {
                m_camera.moveCamera(Camera::Movement::LEFT, static_cast<float>(win.getDeltaTime()));
            })
            .addKeyEventHandler(GLFW_KEY_D, 0, CONTINUOUS, [this](window::Window& win) {
                m_camera.moveCamera(Camera::Movement::RIGHT, static_cast<float>(win.getDeltaTime()));
            })
            .addKeyEventHandler(GLFW_KEY_LEFT_SHIFT, 0, CONTINUOUS, [this](window::Window& win) {
                m_camera.moveCamera(Camera::Movement::DOWNWARD, static_cast<float>(win.getDeltaTime()));
            })
            .addKeyEventHandler(GLFW_KEY_SPACE, 0, CONTINUOUS, [this](window::Window& win) {
                m_camera.moveCamera(Camera::Movement::UPWARD, static_cast<float>(win.getDeltaTime()));
            })

            // camera control
            .setScrollCallback([this](window::Window& window, double /* xOffset */, double yOffset) {
                if (window.isMouseCaptured()) { m_camera.updatePerspective(static_cast<float>(yOffset)); }
            })
            .setCursorPosCallback([this](window::Window& window, double xPos, double yPos) {
                auto& winProp{ window.getProperties() };
                auto& lastX{ winProp.m_cursorPos.x };
                auto& lastY{ winProp.m_cursorPos.y };

                float xoffset = static_cast<float>(xPos - lastX);
                float yoffset = static_cast<float>(lastY - yPos);
                lastX         = xPos;
                lastY         = yPos;

                if (window.isMouseCaptured()) {
                    m_camera.lookAround(xoffset, yoffset);
                }
            });
    }
};
