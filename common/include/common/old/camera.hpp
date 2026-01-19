#ifndef CAMERA_H
#define CAMERA_H

#include <algorithm>
#include <cmath>
#include <numbers>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera
{
private:
    // default values for camera
    static constexpr float DEFAULT_PITCH{ 0.0f };
    static constexpr float DEFAULT_YAW{ 360.0f - 90.0f };
    static constexpr float DEFAULT_FOV{ 45.0f };
    static constexpr float DEFAULT_SPEED{ 2.5f };
    static constexpr float DEFAULT_SENSITIVITY{ 1.0f };

public:
    enum class Movement
    {
        FORWARD,
        BACKWARD,
        RIGHT,
        LEFT,
        UPWARD,
        DOWNWARD,
    };

    struct InitParam
    {
        glm::vec3 position    = { 0.0f, 0.0f, 3.0f };
        glm::vec3 worldUp     = { 0.0f, 1.0f, 0.0f };
        float     pitch       = DEFAULT_PITCH;
        float     yaw         = DEFAULT_YAW;
        float     fov         = DEFAULT_FOV;
        float     speed       = DEFAULT_SPEED;
        float     sensitivity = DEFAULT_SENSITIVITY;
    };

public:
    // camera vectors
    glm::vec3 m_position;
    glm::vec3 m_worldUp;            // used for up and down movement
    glm::vec3 m_up;                 // calculated later
    glm::vec3 m_right;              // calculated later
    glm::vec3 m_front;              // calculated later
    glm::vec3 m_horizontalFront;    // calcualted later; used for forth and back movement

    // euler angles
    float m_pitch;    // in degrees
    float m_yaw;      // in degrees

    // camera attributes
    float m_fov;
    float m_speed;
    float m_sensitivity;

    // near and far plane
    float m_near{ 0.01f };
    float m_far{ 100.0f };

public:
    // constructor with vectors
    Camera(InitParam&& init)
        : m_position{ init.position }
        , m_worldUp{ init.worldUp }
        , m_pitch{ init.pitch }
        , m_yaw{ init.yaw }
        , m_fov{ init.fov }
        , m_speed{ init.speed }
        , m_sensitivity{ init.sensitivity }
    {
        updateCameraVector();
    }

    // return view matrix
    glm::mat4 getViewMatrix() { return glm::lookAt(m_position, m_position + m_front, m_up); }

    glm::mat4 getProjectionMatrix(int width, int height)
    {
        return glm::perspective(
            glm::radians(m_fov), static_cast<float>(width) / static_cast<float>(height), m_near, m_far
        );
    }

    void setNear(float near) { m_near = near; }

    void setFar(float far) { m_far = far; }

    // process camera movement
    void moveCamera(Movement movement, float deltaTime, bool stickToGround = true)
    {
        switch (movement) {
        case Movement::FORWARD:
            if (stickToGround) {
                m_position += m_horizontalFront * m_speed * deltaTime;
            } else {
                m_position += m_front * m_speed * deltaTime;
            }
            break;
        case Movement::BACKWARD:
            if (stickToGround) {
                m_position -= m_horizontalFront * m_speed * deltaTime;
            } else {
                m_position -= m_front * m_speed * deltaTime;
            }
            break;
        case Movement::RIGHT: m_position += m_right * m_speed * deltaTime; break;
        case Movement::LEFT: m_position -= m_right * m_speed * deltaTime; break;
        case Movement::UPWARD: m_position += m_worldUp * m_speed * deltaTime; break;
        case Movement::DOWNWARD: m_position -= m_worldUp * m_speed * deltaTime; break;
        default: break;
        }
    }

    // process 3D rotation from mouse
    void lookAround(float xOffset, float yOffset)
    {
        xOffset *= m_sensitivity * 0.1f;
        yOffset *= m_sensitivity * 0.1f;

        m_yaw   = std::fmod(360.0f + m_yaw + xOffset, 360.0f);    // how to keep it in range [0, 360]?
        m_pitch = std::clamp(m_pitch + yOffset, -89.0f, 89.0f);

        updateCameraVector();
    }

    // process scroll (usually)
    void updatePerspective(float yOffset)
    {
        m_fov += -static_cast<float>(yOffset) * m_sensitivity;
        m_fov  = std::clamp(m_fov, 1.0f, 180.0f);
    }

    // reset look, to origin
    void lookAtOrigin()
    {
        using namespace std::numbers;

        const auto& direction{ -m_position };    // direction of camera = origin - camera.position;
                                                 // origin at (0,0,0)

        m_yaw = 180.0f / pi_v<float> * std::atan(direction.z / direction.x);    // returns -90 to 90

        // for some reason direction.x < 0 doesn't work, we need to add some error (in this case 1e-5)
        if (direction.x < 1e-5f) {    // handle condition when camera already looked at origin
            m_yaw += 180.0f;
        }

        m_pitch = 180.0f / pi_v<float>
                * std::atan(direction.y / std::sqrt(direction.x * direction.x + direction.z * direction.z));

        updateCameraVector();
    }

private:
    void updateCameraVector()
    {
        glm::vec3 direction{
            std::cos(glm::radians(m_pitch)) * std::cos(glm::radians(m_yaw)),
            std::sin(glm::radians(m_pitch)),
            std::cos(glm::radians(m_pitch)) * std::sin(glm::radians(m_yaw)),
        };

        m_front = glm::normalize(direction);
        m_right = glm::normalize(glm::cross(m_front, m_worldUp));
        m_up    = glm::normalize(glm::cross(m_right, m_front));

        // horizontal front y value is zero (only in xz plane aka horizontal)
        m_horizontalFront = glm::normalize(glm::vec3(direction.x, 0, direction.z));
    }
};

#endif
