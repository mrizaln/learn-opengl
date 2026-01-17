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
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/norm.hpp>    // glm::length2

#include <glbinding/gl/gl.h>

#include "window.hpp"
#include "window_manager.hpp"
#include "cube.hpp"
#include "plane.hpp"
#include "camera.hpp"
#include "shader.hpp"
#include "image_texture.hpp"
#include "stringified_enum.hpp"
#include "scope_time_logger.hpp"

#include "util/assets_path.hpp"

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
    std::string  m_name;
    ImageTexture m_diffuse;
    ImageTexture m_specular;
    float        m_shininess;

    Material(
        const std::string&    name,
        std::filesystem::path diffuseMap,
        std::filesystem::path specularMap,
        float                 shininess
    )
        : m_name{ name }
        , m_diffuse{ ImageTexture::from(diffuseMap, m_name + ".m_diffuse", 0).value() }       // unwrap
        , m_specular{ ImageTexture::from(specularMap, m_name + ".m_specular", 1).value() }    // unwrap
        , m_shininess{ shininess }
    {
    }

    void applyUniform(Shader& shader) const
    {
        m_diffuse.activate(shader);
        m_specular.activate(shader);
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

    static inline auto s_assets_path = util::assets_path("4.01_depth_testing");

    // clang-format off
    static inline constexpr std::array<glm::vec3, 2> s_cubePositions{ {
        { -1.0f, 0.0f, -1.0f },
        {  2.0f, 0.0f,  0.0f },
    } };

    static inline constexpr std::size_t s_numPointLights{ 4 };
    static inline constexpr std::array<glm::vec3, s_numPointLights> s_pointLightsPositions{ {
        {  1.2f, 0.2f,  3.0f  },
        {  2.3f, 1.3f, -4.0f  },
        { -4.0f, 2.0f, -7.0f },
        {  0.0f, 0.0f, -7.2f  },
    } };

    static inline constexpr glm::vec3 s_floorPosition{ 0.0f, -0.5f, 0.0f };
    // clang-format on

    window::Window& m_window;
    glm::vec3       m_backgroundColor;

    Camera                                   m_camera;
    Shader                                   m_shader;
    Shader                                   m_lightShader;
    Cube                                     m_cube;
    Cube                                     m_lightCube;
    Plane                                    m_floor;
    Material                                 m_cubeMaterial;
    Material                                 m_floorMaterial;
    DirectionalLight                         m_directionalLight;
    std::array<PointLight, s_numPointLights> m_pointLights;
    SpotLight                                m_spotLight;

    bool m_drawWireFrame{ false };
    bool m_invertRender{ false };
    bool m_rotate{ false };

    UniformData<LightsUsed> u_activatedLights;
    UniformData<float&>     u_nearPlane;
    UniformData<float&>     u_farPlane;
    UniformData<bool>       u_enableColorOutput;
    UniformData<bool>       u_enableDepthOutput;
    UniformData<bool>       u_invertDepthOutput;

public:
    Scene()                        = delete;
    Scene(const Scene&)            = delete;
    Scene& operator=(const Scene&) = delete;
    Scene(Scene&&)                 = delete;
    Scene& operator=(Scene&&)      = delete;

    Scene(window::Window& window)
        : m_window{ window }
        , m_backgroundColor{ 0.1f, 0.1f, 0.2f }
        , m_camera{ {} }
        , m_shader{
            s_assets_path / "shader/shader.vert",
            s_assets_path / "shader/shader.frag",
        }
        , m_lightShader{
            s_assets_path / "shader/shader.vert",
            s_assets_path / "shader/light_shader.frag",
        }
        , m_cube{ 1.0f }
        , m_lightCube{ 0.2f }
        , m_floor{ 15.0f }
        , m_cubeMaterial{
            /* .m_name      = */ "u_material",
            /* .m_diffuse   = */ s_assets_path / "texture/metal.png",
            /* .m_specular  = */ s_assets_path / "texture/metal.png",
            /* .m_shininess = */ 128.0f,
        }
        , m_floorMaterial{
            /* .m_name      = */ "u_material",
            /* .m_diffuse   = */ s_assets_path / "texture/marble.jpg",
            /* .m_specular  = */ s_assets_path / "texture/marble.jpg",
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
        , u_activatedLights{ "u_enabledLightsFlag", LightsUsed::LIGHT_POINT }
        , u_nearPlane{ "u_nearPlane", m_camera.m_near }
        , u_farPlane{ "u_farPlane", m_camera.m_far }
        , u_enableColorOutput{ "u_enableColorOutput", true }
        , u_enableDepthOutput{ "u_enableDepthOutput", true }
        , u_invertDepthOutput{ "u_invertDepthOutput", false }
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
        gl::glEnable(gl::GL_DEPTH_TEST);

        m_shader.use();
        m_directionalLight.applyUniforms(m_shader);
        m_spotLight.applyUniforms(m_shader);
        for (auto& light : m_pointLights) { light.applyUniforms(m_shader); }

        m_shader.setUniform(u_activatedLights.m_name, u_activatedLights.m_value.base());
        m_shader.setUniform(u_nearPlane.m_name, u_nearPlane.m_value);
        m_shader.setUniform(u_farPlane.m_name, u_farPlane.m_value);
        m_shader.setUniform(u_enableColorOutput.m_name, u_enableColorOutput.m_value);
        m_shader.setUniform(u_enableDepthOutput.m_name, u_enableDepthOutput.m_value);
        m_shader.setUniform(u_invertDepthOutput.m_name, u_invertDepthOutput.m_value);
    }

    void updateUniforms()
    {
        m_shader.use();
        m_directionalLight.applyUniforms(m_shader);
        m_spotLight.applyUniforms(m_shader);
        for (auto& light : m_pointLights) { light.applyUniforms(m_shader); }

        m_shader.setUniform(u_activatedLights.m_name, u_activatedLights.m_value.base());
    }

    void render()
    {
        PRETTY_FUNCTION_TIME_LOG();

        // clear buffers and update viewport
        gl::glClearColor(m_backgroundColor.r, m_backgroundColor.g, m_backgroundColor.b, 1.0f);
        gl::glClear(gl::GL_COLOR_BUFFER_BIT | gl::GL_DEPTH_BUFFER_BIT);
        const auto& winProp{ m_window.getProperties() };
        gl::glViewport(0, 0, winProp.m_width, winProp.m_height);

        auto view{ m_camera.getViewMatrix() };
        auto projection{ m_camera.getProjectionMatrix(m_window.getProperties().m_width, m_window.getProperties().m_height) };

        updateUniforms();

        drawCube(view, projection);
        drawFloor(view, projection);

        if (u_activatedLights.m_value.test(LightsUsed::LIGHT_POINT)) {
            drawLights(view, projection);
        }
    }

    void setColorOutput(bool value)
    {
        u_enableColorOutput.m_value = value;
        m_shader.use();
        m_shader.setUniform(u_enableColorOutput.m_name, u_enableColorOutput.m_value);
    }

    void setDepthOutput(bool value)
    {
        u_enableDepthOutput.m_value = value;
        m_shader.use();
        m_shader.setUniform(u_enableDepthOutput.m_name, u_enableDepthOutput.m_value);
    }

    void invertDepthOutput(bool value)
    {
        u_invertDepthOutput.m_value = value;
        m_shader.use();
        m_shader.setUniform(u_invertDepthOutput.m_name, u_invertDepthOutput.m_value);
    }

private:
    void drawCube(const glm::mat4& view, const glm::mat4 projection)
    {
        m_shader.use();
        m_shader.setUniform("u_viewPos", m_camera.m_position);
        m_shader.setUniform("u_view", view);
        m_shader.setUniform("u_projection", projection);
        m_cubeMaterial.applyUniform(m_shader);

        for (const auto& pos : s_cubePositions) {
            auto model{ glm::translate(glm::mat4{ 1.0f }, pos) };
            m_shader.setUniform("u_model", model);

            m_cube.draw();
        }
    }
    void drawFloor(const glm::mat4& view, const glm::mat4 projection)
    {
        m_shader.use();
        m_shader.setUniform("u_viewPos", m_camera.m_position);
        m_shader.setUniform("u_view", view);
        m_shader.setUniform("u_projection", projection);
        m_floorMaterial.applyUniform(m_shader);

        auto model{ glm::translate(glm::mat4{ 1.0f }, s_floorPosition) };
        m_shader.setUniform("u_model", model);

        m_floor.draw();
    }

    void drawLights(const glm::mat4& view, const glm::mat4 projection)
    {
        m_lightShader.use();
        m_lightShader.setUniform("u_view", view);
        m_lightShader.setUniform("u_projection", projection);

        for (auto& light : m_pointLights) {
            light.setLightColor("u_lightColor", m_lightShader);

            if (m_rotate) {
                auto      transform = glm::rotate(glm::mat4{ 1.0f }, 0.5f / glm::length2(light.m_position), { 0.0f, 1.0f, 0.0f });    // rotate around y-axis
                glm::vec4 newPos{ transform * glm::vec4{ light.m_position, 1.0f } };
                light.m_position = { newPos.x, newPos.y, newPos.z };
            }

            glm::mat4 model{ 1.0f };
            model = glm::translate(model, light.m_position);
            m_lightShader.setUniform("u_model", model);

            m_lightCube.draw();
        }
    }

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
