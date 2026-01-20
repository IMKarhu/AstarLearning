#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

class Camera
{
public:
    Camera(int width, int height, glm::vec3 position)
        : m_position(position)
    {
        m_modelMatrix = glm::mat4(1.0f);
        m_viewMatrix = glm::lookAt(
            m_position,
            m_target,
            m_up
        );
        m_projectionMatrix = glm::perspective(
            glm::radians(45.0f),
            (float)width/(float)height,
            0.1f,
            100.0f
        );
    }

    void update()
    {
        m_position = m_target + glm::vec3(
            m_distance * cos(glm::radians(m_pitch)) * sin(glm::radians(m_yaw)),
            m_distance * sin(glm::radians(m_pitch)),
            m_distance * cos(glm::radians(m_pitch)) * cos(glm::radians(m_yaw))
        );

        m_viewMatrix = glm::lookAt(
            m_position,
            m_target,
            m_up
        );
    }
    void processMouse(float x, float y, float sensitivity)
    {
        m_yaw -= x * sensitivity;
        m_pitch += y * sensitivity;
        m_pitch = glm::clamp(m_pitch, 1.0f, 89.0f);
    }

    void processScroll(float yoffset)
    {
        m_distance -= yoffset;
        m_distance = glm::clamp(m_distance, 10.0f, 50.0f);
    }

    void resize(int width, int height)
    {
        m_projectionMatrix = glm::perspective(glm::radians(45.0f),
            (float)width / (float)height,
            0.1f,
            100.0f);
    }

    glm::vec3 position() const { return m_position; }
    glm::mat4 model() { return m_modelMatrix; }
    glm::mat4 view() { return m_viewMatrix; }
    glm::mat4 proj() { return m_projectionMatrix; }
private:
    glm::vec3 m_position;
    glm::vec3 m_target = glm::vec3(0.0f);
    glm::vec3 m_up = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::mat4 m_modelMatrix;
    glm::mat4 m_viewMatrix;
    glm::mat4 m_projectionMatrix;

    float m_distance = 10.0f;
    float m_yaw = 45.0f;
    float m_pitch = 45.0f;
};