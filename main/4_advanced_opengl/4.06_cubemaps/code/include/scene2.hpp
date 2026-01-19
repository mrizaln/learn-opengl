#ifndef SCENE2_HPP_STZR6ITN
#define SCENE2_HPP_STZR6ITN

#include <algorithm>
#include <array>
#include <atomic>
#include <cmath>
#include <concepts>
#include <cstdint>
#include <format>
#include <glm/ext/matrix_transform.hpp>
#include <iostream>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glbinding/gl/gl.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/norm.hpp>    // glm::length2

#include "common/old/camera.hpp"
#include "common/old/cube.hpp"
#include "common/old/cubemap.hpp"
#include "common/old/framebuffer.hpp"
#include "common/old/image_texture.hpp"
#include "common/old/opengl_option_stack.hpp"
#include "common/old/plane.hpp"
#include "common/old/scope_time_logger.hpp"
#include "common/old/shader.hpp"
#include "common/old/stringified_enum.hpp"
#include "common/old/window.hpp"
#include "common/old/window_manager.hpp"
#include "common/util/assets_path.hpp"

class ImGuiLayer;

class Scene2
{
private:
    friend ImGuiLayer;

    template <typename T>
    struct UniformData
    {
        std::string m_name;
        T           m_value;
    };

    static inline auto s_assets_path = util::assets_path("4.06_cubemaps");

    static inline constexpr std::array<glm::vec3, 2> s_cubePositions{ {
        { -2.0f, 0.0f, 0.0f },
        { 2.0f, 0.0f, 0.0f },
    } };

private:
    window::Window&   m_window;
    Framebuffer       m_framebuffer;
    glm::vec3         m_backgroundColor;
    OpenGLOptionStack m_optionStack;

    Camera  m_camera;
    Shader  m_reflectionShader;
    Shader  m_refractionShader;
    Shader  m_ndcShader;
    Shader  m_skyboxShader;
    Cube    m_cube;
    Plane   m_screenPlane;
    Cubemap m_skybox;

    bool m_drawWireFrame{ false };
    bool m_invertRender{ false };
    bool m_rotate{ false };

public:
    Scene2()                         = delete;
    Scene2(const Scene2&)            = delete;
    Scene2& operator=(const Scene2&) = delete;
    Scene2(Scene2&&)                 = delete;
    Scene2& operator=(Scene2&&)      = delete;

    Scene2(window::Window& window)
        : m_window{ window }
        , m_framebuffer{ Framebuffer::create(window.getProperties().m_width, window.getProperties().m_height).value() }    // skip optional check
        , m_backgroundColor{ 0.1f, 0.1f, 0.2f }
        , m_camera{ {} }
        , m_reflectionShader{
            s_assets_path / "shader/shader.vert",
            s_assets_path / "shader/reflection.frag",
        }
        , m_refractionShader{
            s_assets_path / "shader/shader.vert",
            s_assets_path / "shader/refraction.frag",
        }
        , m_ndcShader{
            s_assets_path / "shader/ndc_shader.vert",
            s_assets_path / "shader/ndc_shader.frag",
        }
        , m_skyboxShader{
            s_assets_path / "shader/skybox.vert",
            s_assets_path / "shader/skybox.frag",
        }
        , m_cube{ 1.0f }
        , m_screenPlane{ 2.0f }
        , m_skybox{ [] {
            Cubemap::CubeImagePath imagePath{
                .right  = s_assets_path / "texture/skybox/right.jpg",
                .left   = s_assets_path / "texture/skybox/left.jpg",
                .top    = s_assets_path / "texture/skybox/top.jpg",
                .bottom = s_assets_path / "texture/skybox/bottom.jpg",
                .back   = s_assets_path / "texture/skybox/back.jpg",
                .front  = s_assets_path / "texture/skybox/front.jpg",
            };
            return Cubemap::from(std::move(imagePath), "u_skybox", 0).value();    // skip optional check
        }() }
    {
        setWindowEventsHandler();
    }

public:
    void init()
    {
        m_framebuffer.use([this]() {
            m_reflectionShader.use();

            // depth
            gl::glEnable(gl::GL_DEPTH_TEST);

            // blending
            gl::glEnable(gl::GL_BLEND);
            gl::glBlendFunc(gl::GL_SRC_ALPHA, gl::GL_ONE_MINUS_SRC_ALPHA);

            // face culling
            gl::glEnable(gl::GL_CULL_FACE);
        });
    }

    void render()
    {
        m_framebuffer.use([this]() {
            renderScene();
        });
        drawFramebuffer();
    }

private:
    void drawFramebuffer()
    {
        gl::glClear(gl::GL_COLOR_BUFFER_BIT | gl::GL_DEPTH_BUFFER_BIT | gl::GL_STENCIL_BUFFER_BIT);

        m_optionStack.push();
        m_optionStack.loadDefaults();

        m_ndcShader.use();
        m_framebuffer.bindTexture();
        m_screenPlane.draw();

        m_optionStack.pop();
    }

    void drawCube(const glm::mat4& view, const glm::mat4& projection)
    {
        static double lastTime{ 0.0 };    // yeah, this local static variable is not good, but eh, once is ok
        if (m_rotate) { lastTime += m_window.getDeltaTime(); }

        glm::vec3 rotationAxis{ std::sin(lastTime * 2 + 60), std::cos(lastTime / 100), std::atan(lastTime) };
        glm::mat4 model;

        // reflection
        auto reflectCubePos{ s_cubePositions[0] };
        m_reflectionShader.use();
        m_reflectionShader.setUniform("u_viewPos", m_camera.m_position);
        m_reflectionShader.setUniform("u_view", view);
        m_reflectionShader.setUniform("u_projection", projection);

        model = glm::mat4{ 1.0f };
        model = glm::translate(model, reflectCubePos);
        model = glm::rotate(model, (float)lastTime, rotationAxis);
        m_reflectionShader.setUniform("u_model", model);
        m_skybox.activate(m_reflectionShader);
        m_cube.draw();

        // refraction
        auto refractCubePos{ s_cubePositions[1] };
        m_refractionShader.use();
        m_refractionShader.setUniform("u_viewPos", m_camera.m_position);
        m_refractionShader.setUniform("u_view", view);
        m_refractionShader.setUniform("u_projection", projection);

        model = glm::mat4{ 1.0f };
        model = glm::translate(model, refractCubePos);
        model = glm::rotate(model, (float)lastTime, rotationAxis);
        m_refractionShader.setUniform("u_model", model);
        m_skybox.activate(m_refractionShader);
        m_cube.draw();
    }

    void drawSkybox(const glm::mat4& view, const glm::mat4& projection)
    {
        m_optionStack.push();
        m_optionStack.loadDefaults();

        glm::mat4 viewWithoutTranslation{ glm::mat3{ view } };

        m_skyboxShader.use();
        m_skyboxShader.setUniform("u_view", viewWithoutTranslation);
        m_skyboxShader.setUniform("u_projection", projection);
        m_skybox.activate(m_skyboxShader);

        m_cube.draw();

        m_optionStack.pop();
    }

    void renderScene()
    {
        PRETTY_FUNCTION_TIME_LOG();

        // clear buffers and update viewport
        gl::glClearColor(m_backgroundColor.r, m_backgroundColor.g, m_backgroundColor.b, 1.0f);
        gl::glClear(gl::GL_COLOR_BUFFER_BIT | gl::GL_DEPTH_BUFFER_BIT);

        const auto& winProp{ m_window.getProperties() };
        gl::glViewport(0, 0, winProp.m_width, winProp.m_height);

        auto view{ m_camera.getViewMatrix() };
        auto projection{ m_camera.getProjectionMatrix(winProp.m_width, winProp.m_height) };

        drawSkybox(view, projection);
        drawCube(view, projection);
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
                    gl::glCullFace(gl::GL_FRONT);
                } else {
                    gl::glCullFace(gl::GL_BACK);
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
                if (glfwGetKey(window.getHandle(), GLFW_KEY_LEFT_ALT) != GLFW_RELEASE) {
                    m_camera.updatePerspective(static_cast<float>(yOffset));
                } else {
                    constexpr float multiplier{ 1.1f };
                    m_camera.m_speed = yOffset < 0 ? m_camera.m_speed * multiplier : m_camera.m_speed / multiplier;
                }
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

        // framebuffer resize
        m_window
            .setFramebuffersizeCallback([this](window::Window& /* window */, int width, int height) {
                gl::glViewport(0, 0, width, height);
                m_framebuffer.resize(width, height);
            });
    }
};

#endif /* end of include guard: SCENE2_HPP_STZR6ITN */
