#ifndef SCENE_HPP_XFKJA3VZ
#define SCENE_HPP_XFKJA3VZ

#include <array>
#include <atomic>
#include <cmath>
#include <cstdint>
#include <format>
#include <iostream>
#include <optional>
#include <thread>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <glbinding/gl/gl.h>

#include "window.hpp"
#include "window_manager.hpp"
#include "cube.hpp"
#include "camera.hpp"
#include "shader.hpp"
#include "texture.hpp"
#include "stringified_enum.hpp"
#include "scope_time_logger.hpp"

#define _UNIFORM_FIELD_EXPANDER(type, name) type name;
#define _UNIFORM_APPLY_EXPANDER(type, name) shader.setUniform(m_name + "." #name, name);
#define UNIFORM_STRUCT_CREATE(FIELDS)        \
    std::string m_name;                      \
                                             \
    FIELDS(_UNIFORM_FIELD_EXPANDER)          \
                                             \
    void applyUniforms(Shader& shader) const \
    {                                        \
        FIELDS(_UNIFORM_APPLY_EXPANDER)      \
    }

template <typename T>
struct UniformData
{
    std::string m_name;
    T           m_value;
};

struct Material
{
    std::string m_name;
    Texture     m_diffuse;
    Texture     m_specular;
    Texture     m_emission;
    float       m_shininess;

    Material(
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

struct DirectionalLight
{
#define FIELDS(M)             \
    M(glm::vec3, m_direction) \
    M(glm::vec3, m_ambient)   \
    M(glm::vec3, m_diffuse)   \
    M(glm::vec3, m_specular)

    UNIFORM_STRUCT_CREATE(FIELDS);
#undef FIELDS
};

struct PointLight
{
#define FIELDS(M)            \
    M(glm::vec3, m_position) \
    M(glm::vec3, m_ambient)  \
    M(glm::vec3, m_diffuse)  \
    M(glm::vec3, m_specular) \
    M(float, m_constant)     \
    M(float, m_linear)       \
    M(float, m_quadratic)

    UNIFORM_STRUCT_CREATE(FIELDS);

    void setLightColor(const std::string& name, Shader& lightShader) { lightShader.setUniform(name, m_specular); }    // dirty quick hack
#undef FIELDS
};

struct SpotLight
{
#define FIELDS(M)                                                  \
    M(glm::vec3&, m_position)  /* reference to camera position */  \
    M(glm::vec3&, m_direction) /* reference to camera direction */ \
    M(glm::vec3, m_ambient)                                        \
    M(glm::vec3, m_diffuse)                                        \
    M(glm::vec3, m_specular)                                       \
    M(float, m_cutOff)                                             \
    M(float, m_outerCutOff)                                        \
    M(float, m_constant)                                           \
    M(float, m_linear)                                             \
    M(float, m_quadratic)

    UNIFORM_STRUCT_CREATE(FIELDS);
#undef FIELDS
};

#define ENUM_FIELDS(M)   \
    M(LIGHT_DIRECTIONAL) \
    M(LIGHT_POINT)       \
    M(LIGHT_SPOT)
using LightsUsed = STRINGIFIED_ENUM_FLAG(LightsUsed, unsigned int, ENUM_FIELDS);
#undef ENUM_FIELDS

class ImGuiLayer;

class Scene
{
private:
    friend ImGuiLayer;

    // clang-format off
    static inline constexpr std::array<glm::vec3, 10> s_cubePositions{ {
        {  0.0f,  0.0f,  0.0f  },
        {  2.0f,  5.0f, -15.0f },
        { -1.5f, -2.2f, -2.5f  },
        { -3.8f, -2.0f, -12.3f },
        {  2.4f, -0.4f, -3.5f  },
        { -1.7f,  3.0f, -7.5f  },
        {  1.3f, -2.0f, -2.5f  },
        {  1.5f,  2.0f, -2.5f  },
        {  1.5f,  0.2f, -1.5f  },
        { -1.3f,  1.0f, -1.5f  },
    } };

    static inline constexpr std::size_t s_numPointLights{ 4 };
    static inline constexpr std::array<glm::vec3, s_numPointLights> s_pointLightsPositions{ {
        {  0.7f,  0.2f,  2.0f  },
        {  2.3f, -3.3f, -4.0f  },
        { -4.0f,  2.0f, -12.0f },
        {  0.0f,  0.0f, -3.0f  },
    } };
    // clang-format on

    window::Window&                          m_window;
    Camera                                   m_camera;
    Shader                                   m_shader;
    Shader                                   m_lightShader;
    Cube                                     m_cube;
    Cube                                     m_lightCube;
    Material                                 m_material;
    DirectionalLight                         m_directionalLight;
    std::array<PointLight, s_numPointLights> m_pointLights;
    SpotLight                                m_spotLight;

    UniformData<LightsUsed> u_activatedLights{ "u_enabledLightsFlag", LightsUsed::ALL };

    // options
    bool m_drawWireFrame{ false };
    bool m_invertRender{ false };
    bool m_rotate{ false };
    bool m_enableEmissionMap{ false };

public:
    Scene()                        = delete;
    Scene(const Scene&)            = delete;
    Scene& operator=(const Scene&) = delete;
    Scene(Scene&&)                 = delete;
    Scene& operator=(Scene&&)      = delete;

    Scene(window::Window& window)
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
        , m_directionalLight{
            .m_name      = "u_directionalLight",
            .m_direction = { -0.2f, -1.0f, -0.3f },
            .m_ambient   = { 0.2f, 0.2f, 0.2f },
            .m_diffuse   = { 0.5f, 0.5f, 0.5f },
            .m_specular  = { 1.0f, 1.0f, 1.0f },
        }
        , m_pointLights{ /* to be filled later */ }
        , m_spotLight{
            .m_name        = "u_spotLight",
            .m_position    = m_camera.m_position,
            .m_direction   = m_camera.m_front,
            .m_ambient     = m_directionalLight.m_ambient,
            .m_diffuse     = m_directionalLight.m_diffuse,
            .m_specular    = m_directionalLight.m_specular,
            .m_cutOff      = glm::cos(glm::radians(12.5f)),
            .m_outerCutOff = glm::cos(glm::radians(15.0f)),
            .m_constant    = 1.0f,
            .m_linear      = 0.09f,
            .m_quadratic   = 0.032f,
        }
    {

        for (std::size_t i{ 0 }; i < s_numPointLights; ++i) {
            m_pointLights[i] = {
                .m_name      = std::format("u_pointLight[{}]", i),
                .m_position  = s_pointLightsPositions[i],
                .m_ambient   = m_directionalLight.m_ambient,
                .m_diffuse   = m_directionalLight.m_diffuse,
                .m_specular  = m_directionalLight.m_specular,
                .m_constant  = 1.0f,
                .m_linear    = 0.09f,
                .m_quadratic = 0.032f,
            };
        }

        m_window.setClearColor(0.1f, 0.1f, 0.2f);
        setWindowEventsHandler();
    }

public:
    void readDeviceInformation()
    {
        m_window.useHere();

        // device information
        auto vendor{ glGetString(gl::GL_VENDOR) };        // Returns the vendor
        auto renderer{ glGetString(gl::GL_RENDERER) };    // Returns a hint to the model
        std::cout << '\n';
        std::cout << "Device: " << renderer << '\n';
        std::cout << "Vendor: " << vendor << '\n';

        // int nrAttributes;
        // glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &nrAttributes);
        // std::cout << "Maximum number of vertex attributes supported: " << nrAttributes << '\n';

        std::cout << '\n';

        m_window.unUse();
    }

    void init()
    {
        m_shader.use();
        m_material.applyUniform(m_shader);
        m_directionalLight.applyUniforms(m_shader);
        m_spotLight.applyUniforms(m_shader);
        for (auto& light : m_pointLights) { light.applyUniforms(m_shader); }
        m_shader.setUniform(u_activatedLights.m_name, u_activatedLights.m_value.base());
    }

    void render()
    {
        PRETTY_FUNCTION_TIME_LOG();

        auto      view{ m_camera.getViewMatrix() };
        auto      projection{ m_camera.getProjectionMatrix(m_window.getProperties().m_width, m_window.getProperties().m_height) };
        glm::mat4 model{ 1.0f };

        m_shader.use();
        m_material.applyUniform(m_shader);
        m_directionalLight.applyUniforms(m_shader);
        m_spotLight.applyUniforms(m_shader);
        for (auto& light : m_pointLights) { light.applyUniforms(m_shader); }
        m_shader.setUniform(u_activatedLights.m_name, u_activatedLights.m_value.base());

        //----------------[ light cube object ]-----------------
        m_lightShader.use();
        m_lightShader.setUniform("u_view", view);
        m_lightShader.setUniform("u_projection", projection);
        for (auto& light : m_pointLights) { light.setLightColor("u_lightColor", m_lightShader); }

        for (auto& light : m_pointLights) {
            model = glm::mat4{ 1.0f };
            model = glm::translate(glm::mat4{ 1.0f }, light.m_position);
            model = glm::scale(model, glm::vec3(0.2f));
            m_lightShader.setUniform("u_model", model);

            m_lightCube.draw();
        }
        //------------------------------------------------------

        //----------------[ cube container object ]-----------------
        m_shader.use();
        m_shader.setUniform("u_viewPos", m_camera.m_position);
        m_shader.setUniform("u_view", view);
        m_shader.setUniform("u_projection", projection);

        static double lastTime{ 0.0 };    // yeah, this local static variable is not good, but eh, once is ok
        if (m_rotate) { lastTime += m_window.getDeltaTime(); }

        for (int i{ 0 }; const auto& pos : s_cubePositions) {
            auto      offset{ i++ };
            glm::vec3 rotationAxis{
                std::sin(lastTime * (2 + offset % 3) + 60 * offset),
                std::cos(lastTime / (100 * (1 + offset % 3))),
                std::atan(lastTime)
            };
            auto model{ glm::translate(glm::mat4{ 1.0f }, pos) };
            model = glm::rotate(model, (float)lastTime, glm::normalize(rotationAxis));
            m_shader.setUniform("u_model", model);

            m_cube.draw();
        }
        //----------------------------------------------------------
    }

private:
    void setWindowEventsHandler()
    {
        using enum window::Window::KeyActionType;

        // various control
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
                m_window.setVsync(!m_window.isVsyncEnabled());
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
            });

        // camera movements (translation)
        m_window
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
            });

        // camera control (rotation and zoom)
        m_window
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

#endif /* end of include guard: SCENE_HPP_XFKJA3VZ */
