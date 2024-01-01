#ifndef SCENE_HPP_4GQZUVTS
#define SCENE_HPP_4GQZUVTS

#include <numbers>

#include <glbinding/gl/gl.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "window.hpp"
#include "cube.hpp"
#include "camera.hpp"
#include "shader.hpp"
#include "opengl_option_stack.hpp"

namespace
{
    inline long double operator""_deg_to_rad(long double deg) { return std::numbers::pi_v<long double> * deg / 180.0L; }
    inline long double operator""_rad_to_deg(long double rad) { return 180.0L * rad / std::numbers::pi_v<long double>; }
}

class Scene
{
private:
    struct Transformation
    {
        glm::vec3 m_position;
        glm::vec3 m_scale;
        glm::vec3 m_rotationAxis;
        float     m_rotationAngle;    // in radians

        glm::mat4 getModelMatrix() const
        {
            glm::mat4 model{ 1.0f };
            model = glm::translate(model, m_position);
            model = glm::rotate(model, m_rotationAngle, m_rotationAxis);
            model = glm::scale(model, m_scale);
            return model;
        }
    };

    inline static constexpr std::size_t s_numOfCubes{ 1 };

private:
    window::Window&   m_window;
    OpenGLOptionStack m_optionStack;

    Camera m_camera;
    Cube   m_cube;

    std::array<Shader, s_numOfCubes> m_cubeShaders;

public:
    Scene()             = delete;
    Scene(const Scene&) = delete;
    Scene(Scene&&)      = delete;

    Scene(window::Window& window)
        : m_window{ window }
        , m_camera{ {} }
        , m_cube{ 1.0f }
        , m_cubeShaders{ {
              { "./assets/shader/shader.vert", "./assets/shader/points.frag" },
          } }
    {
        configureEventsHandling();

        gl::glClearColor(0.1f, 0.1f, 0.2f, 1.0f);
        gl::glEnable(gl::GL_DEPTH_TEST);

        gl::glEnable(gl::GL_PROGRAM_POINT_SIZE);    // enable influencing the gl_PointSize in vertex shader
    }

public:
    // TODO: add other cube rendering
    void render()
    {
        gl::glClear(gl::GL_COLOR_BUFFER_BIT | gl::GL_DEPTH_BUFFER_BIT);

        Transformation transformation{
            .m_position      = glm::vec3{ 0.0f },
            .m_scale         = glm::vec3{ 1.0f },
            .m_rotationAxis  = { 0.0f, 1.0f, 0.0f },
            .m_rotationAngle = (float)0.0_deg_to_rad,
        };

        for (std::size_t i{ 0 }; i < s_numOfCubes; ++i) {
            auto& shader{ m_cubeShaders[i] };
            shader.use();

            const auto& prop{ m_window.getProperties() };
            shader.setUniform("u_projection", m_camera.getProjectionMatrix(prop.m_width, prop.m_height));
            shader.setUniform("u_view", m_camera.getViewMatrix());
            shader.setUniform("u_model", transformation.getModelMatrix());

            shader.setUniform("u_objectColor", 1.0f, 0.5f, 0.31f);
            shader.setUniform("u_lightColor", 1.0f, 1.0f, 1.0f);

            m_optionStack.push(OpenGLOptionStack::WIREFRAME);

            gl::glPolygonMode(gl::GL_FRONT_AND_BACK, gl::GL_LINE);
            m_cube.draw();

            gl::glPolygonMode(gl::GL_FRONT_AND_BACK, gl::GL_POINT);
            // gl::glPointSize(10.0f);      // point size can be set directly here as well
            m_cube.draw();

            m_optionStack.pop();
        }
    }

private:
    void configureEventsHandling()
    {
        using namespace window;
        using KA = Window::KeyActionType;

        // window resize
        m_window.setFramebuffersizeCallback([](Window&, int width, int height) {
            gl::glViewport(0, 0, width, height);
        });

        // mouse capture
        m_window
            .addKeyEventHandler(GLFW_KEY_C, GLFW_MOD_ALT, KA::CALLBACK, [](Window& window) {
                window.setCaptureMouse(!window.isMouseCaptured());
            });

        // mouse control
        m_window
            .setCursorPosCallback([this](Window& window, double xPos, double yPos) {
                if (!window.isMouseCaptured()) { return; }

                auto& winProp{ window.getProperties() };
                auto& lastX{ winProp.m_cursorPos.x };
                auto& lastY{ winProp.m_cursorPos.y };

                float xOffset = static_cast<float>(xPos - lastX);
                float yOffset = static_cast<float>(lastY - yPos);

                m_camera.lookAround(xOffset, yOffset);
            });

        // scroll control
        m_window
            .setScrollCallback([this](Window&, double, double yOffset) {
                m_camera.updatePerspective((float)yOffset);
                constexpr float multiplier{ 1.01f };
                m_camera.m_speed = yOffset < 0 ? m_camera.m_speed * multiplier : m_camera.m_speed / multiplier;
            });

        // quake and vim style movement
        m_window
            .addKeyEventHandler({ GLFW_KEY_W, GLFW_KEY_K }, 0, KA::CONTINUOUS, [this](Window&) {
                m_camera.moveCamera(Camera::Movement::FORWARD, (float)m_window.getDeltaTime());
            })
            .addKeyEventHandler({ GLFW_KEY_S, GLFW_KEY_J }, 0, KA::CONTINUOUS, [this](Window&) {
                m_camera.moveCamera(Camera::Movement::BACKWARD, (float)m_window.getDeltaTime());
            })
            .addKeyEventHandler({ GLFW_KEY_A, GLFW_KEY_H }, 0, KA::CONTINUOUS, [this](Window&) {
                m_camera.moveCamera(Camera::Movement::LEFT, (float)m_window.getDeltaTime());
            })
            .addKeyEventHandler({ GLFW_KEY_D, GLFW_KEY_L }, 0, KA::CONTINUOUS, [this](Window&) {
                m_camera.moveCamera(Camera::Movement::RIGHT, (float)m_window.getDeltaTime());
            });

        // up and down movement like in minecraft
        m_window
            .addKeyEventHandler(GLFW_KEY_SPACE, 0, KA::CONTINUOUS, [this](Window&) {
                m_camera.moveCamera(Camera::Movement::UPWARD, (float)m_window.getDeltaTime());
            })
            .addKeyEventHandler(GLFW_KEY_LEFT_SHIFT, 0, KA::CONTINUOUS, [this](Window&) {
                m_camera.moveCamera(Camera::Movement::DOWNWARD, (float)m_window.getDeltaTime());
            });
    }
};

#endif /* end of include guard: SCENE_HPP_4GQZUVTS */
