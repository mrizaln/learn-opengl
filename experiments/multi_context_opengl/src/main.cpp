#include <array>
#include <format>
#include <glm/ext/matrix_transform.hpp>
#include <iostream>
#include <optional>
#include <thread>
#include <atomic>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "glbinding/gl/gl.h"
#include "glbinding/glbinding.h"

#include "glbinding-aux/ContextInfo.h"
#include "glbinding-aux/Meta.h"
#include "glbinding-aux/types_to_string.h"
#include "glbinding-aux/ValidVersions.h"
#include "glbinding-aux/debug.h"

#include "window.hpp"
#include "window_manager.hpp"
#include "cube.hpp"
#include "camera.hpp"
#include "shader.hpp"
#include "texture.hpp"

using namespace gl;
using namespace glbinding;

template <typename T>
struct UniformData
{
    std::string name;
    T           value;
};

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

void windowFunction(window::Window&& window, std::array<float, 3> color, bool multiple)
{
    using namespace window;

    window.useHere();

    Cube               cube{};
    Camera             camera({});
    Shader             shader{ "assets/shader/shader.vert", "assets/shader/shader.frag" };
    Texture            texture0{ Texture::from("./assets/texture/container.jpg", "u_texture0", 0).value() };
    Texture            texture1{ Texture::from("./assets/texture/awesomeface.png", "u_texture1", 1).value() };
    UniformData<float> u_mixValue{ "u_mixValue", 0.0f };

    using enum Window::KeyActionType;

    window
        .setClearColor(std::get<0>(color), std::get<1>(color), std::get<2>(color))
        .addKeyEventHandler({ GLFW_KEY_ESCAPE, GLFW_KEY_Q }, 0, CALLBACK, [](Window& win) { win.requestClose(); })
        .addKeyEventHandler(GLFW_KEY_C, GLFW_MOD_ALT, CALLBACK, [](Window& win) {
            win.setCaptureMouse(!win.isMouseCaptured());
        })
        .addKeyEventHandler(GLFW_KEY_W, 0, CONTINUOUS, [&camera](Window& win) {
            camera.moveCamera(Camera::Movement::FORWARD, static_cast<float>((float)win.getDeltaTime()));
        })
        .addKeyEventHandler(GLFW_KEY_S, 0, CONTINUOUS, [&camera](Window& win) {
            camera.moveCamera(Camera::Movement::BACKWARD, static_cast<float>((float)win.getDeltaTime()));
        })
        .addKeyEventHandler(GLFW_KEY_A, 0, CONTINUOUS, [&camera](Window& win) {
            camera.moveCamera(Camera::Movement::LEFT, static_cast<float>((float)win.getDeltaTime()));
        })
        .addKeyEventHandler(GLFW_KEY_D, 0, CONTINUOUS, [&camera](Window& win) {
            camera.moveCamera(Camera::Movement::RIGHT, static_cast<float>((float)win.getDeltaTime()));
        })
        .addKeyEventHandler(GLFW_KEY_LEFT_SHIFT, 0, CONTINUOUS, [&camera](Window& win) {
            camera.moveCamera(Camera::Movement::DOWNWARD, static_cast<float>((float)win.getDeltaTime()));
        })
        .addKeyEventHandler(GLFW_KEY_SPACE, 0, CONTINUOUS, [&camera](Window& win) {
            camera.moveCamera(Camera::Movement::UPWARD, static_cast<float>((float)win.getDeltaTime()));
        })
        .addKeyEventHandler(GLFW_KEY_H, 0, CONTINUOUS, [&shader, &u_mixValue](Window& /* win */) {
            shader.setUniform(u_mixValue.name, (u_mixValue.value = 0.0f));
        })
        .addKeyEventHandler(GLFW_KEY_L, 0, CONTINUOUS, [&shader, &u_mixValue](Window& /* win */) {
            shader.setUniform(u_mixValue.name, (u_mixValue.value = 1.0f));
        })
        .addKeyEventHandler(GLFW_KEY_J, 0, CONTINUOUS, [&shader, &u_mixValue](Window& /* win */) {
            shader.setUniform(u_mixValue.name, (u_mixValue.value = std::clamp(u_mixValue.value - 0.01f, 0.0f, 1.0f)));
        })
        .addKeyEventHandler(GLFW_KEY_K, 0, CONTINUOUS, [&shader, &u_mixValue](Window& /* win */) {
            shader.setUniform(u_mixValue.name, (u_mixValue.value = std::clamp(u_mixValue.value + 0.01f, 0.0f, 1.0f)));
        })
        .setScrollCallback([&camera](Window& window, double /* xOffset */, double yOffset) {
            if (window.isMouseCaptured()) { camera.updatePerspective(static_cast<float>(yOffset)); }
        })
        .setCursorPosCallback([&camera](Window& window, double xPos, double yPos) {
            auto& winProp{ window.getProperties() };
            auto& lastX{ winProp.m_cursorPos.x };
            auto& lastY{ winProp.m_cursorPos.y };

            float xoffset = static_cast<float>(xPos - lastX);
            float yoffset = static_cast<float>(lastY - yPos);
            lastX         = xPos;
            lastY         = yPos;

            if (window.isMouseCaptured()) {
                camera.lookAround(xoffset, yoffset);
            }
        });

    shader.use();
    texture0.activate(shader);
    texture1.activate(shader);
    shader.setUniform(u_mixValue.name, u_mixValue.value);

    window.run([&] {
        auto& prop{ window.getProperties() };
        shader.setUniform("view", camera.getViewMatrix());
        shader.setUniform("projection", camera.getProjectionMatrix(prop.m_width, prop.m_height));

        if (multiple) {
            for (int i{ 0 }; const auto& pos : s_cubePositions) {
                auto      offset{ i++ };
                glm::vec3 rotationAxis{
                    std::sin(glfwGetTime() * (2 + offset % 3) + 60 * offset),
                    std::cos(glfwGetTime() / (100 * (1 + offset % 3))),
                    std::atan(glfwGetTime())
                };
                auto model{ glm::translate(glm::mat4{ 1.0f }, pos) };
                model = glm::rotate(model, (float)glfwGetTime(), glm::normalize(rotationAxis));
                shader.setUniform("model", model);

                // draw cube
                cube.draw();
            }
        } else {
            shader.setUniform("model", [] {
                auto      model{ glm::translate(glm::mat4{ 1.0f }, { 0.0f, 0.0f, 0.0f }) };
                glm::vec3 rotationAxis{
                    std::sin(glfwGetTime() * (2 % 3) + 60),
                    std::cos(glfwGetTime() / (100 * (1 % 3))),
                    std::atan(glfwGetTime())
                };
                return glm::rotate(model, (float)glfwGetTime(), rotationAxis);
            }());
            cube.draw();
        }
    });

    // std::cout << std::this_thread::get_id() << " done\n";
    std::size_t       threadId;
    std::stringstream ss;
    ss << std::this_thread::get_id();
    ss >> threadId;
}

int main(int, char*[])
{
    glfwSetErrorCallback([](int errnum, const char* errmsg) {
        std::cerr << "GLFW error [" << errnum << "]: " << errmsg << std::endl;
    });

    if (!glfwInit()) {
        return 1;
    }
    glfwDefaultWindowHints();

    if (!window::WindowManager::createInstance()) {
        std::cerr << "Failed to create WindowManager instance\n";
        glfwTerminate();
        return 1;
    };

    auto& windowManager{ window::WindowManager::getInstance()->get() };

    auto         window1{ windowManager.createWindow("LearnOpenGL", 800, 600).value() };
    std::jthread window1Thread{ windowFunction, std::move(window1), std::array<float, 3>{ 0.1f, 0.1f, 0.2f }, true };

    auto         window2{ windowManager.createWindow("LearnOpenGL", 800, 600).value() };
    std::jthread window2Thread{ windowFunction, std::move(window2), std::array<float, 3>{ 0.1f, 0.2f, 0.1f }, false };

    auto         window3{ windowManager.createWindow("LearnOpenGL", 800, 600).value() };
    std::jthread window3Thread{ windowFunction, std::move(window3), std::array<float, 3>{ 0.2f, 0.1f, 0.1f }, true };

    auto         window4{ windowManager.createWindow("LearnOpenGL", 800, 600).value() };
    std::jthread window4Thread{ windowFunction, std::move(window4), std::array<float, 3>{ 0.1f, 0.2f, 0.2f }, false };

    while (windowManager.hasWindowOpened()) {
        windowManager.pollEvents(120);    // 2x vsync
        // windowManager.waitEvents();
    }

    window1Thread.join();
    window2Thread.join();
    window3Thread.join();
    window4Thread.join();

    window::WindowManager::destroyInstance();

    glfwTerminate();

    return 0;
}
