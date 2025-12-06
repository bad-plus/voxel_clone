#pragma once

#include "../core/logger.h"

#include <vector>

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

enum Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};

const GLfloat YAW               = -90.f;
const GLfloat PITCH             = 0.0f;
const GLfloat SPEED             = 10.0f;
const GLfloat SENSIVITY         = 0.25f;
const GLfloat ZOOM              = 45.0f;

class Camera {
public:
    glm::vec3 m_position;
    glm::vec3 m_front;
    glm::vec3 m_up;
    glm::vec3 m_right;
    glm::vec3 m_world_up;

    GLfloat m_yaw;
    GLfloat m_pitch;
    GLfloat m_movement_speed;
    GLfloat m_mouse_sensivity;
    GLfloat m_zoom;

    void setSpeed(int speed) {
        m_movement_speed = speed;
    }

    Camera(
        glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
        GLfloat yaw = YAW,
        GLfloat pitch = PITCH
    ) :
        m_front(glm::vec3(0.0f, 0.0f, -1.0f)),
        m_movement_speed(SPEED),
        m_mouse_sensivity(SENSIVITY),
        m_zoom(ZOOM)
    {
        m_position = position;
        m_world_up = up;
        m_yaw = yaw;
        m_pitch = pitch;

        updateCameraVectors();
    }

    Camera(
        GLfloat posX, GLfloat posY, GLfloat posZ,
        GLfloat upX, GLfloat upY, GLfloat upZ,
        GLfloat yaw, GLfloat pitch
    ) :
        m_front(glm::vec3(0.0f, 0.0f, -1.0f)),
        m_movement_speed(SPEED),
        m_mouse_sensivity(SENSIVITY),
        m_zoom(ZOOM)
    {
        m_position = glm::vec3(posX, posY, posZ);
        m_world_up = glm::vec3(upX, upY, upZ);
        m_yaw = yaw;
        m_pitch = pitch;
        
        updateCameraVectors();
    }

    glm::mat4 getViewMatrix() {
        return glm::lookAt(m_position, m_position + m_front, m_up);
    }

    void processKeyboard(Camera_Movement direction, GLfloat delta_time) {
        GLfloat velocity = m_movement_speed * delta_time;

        switch(direction) {
            case FORWARD: {
                m_position += m_front * velocity;
                break;
            }
            case BACKWARD: {
                m_position -= m_front * velocity;
                break;
            }
            case LEFT: {
                m_position -= m_right * velocity;
                break;
            }
            case RIGHT: {
                m_position += m_right * velocity;
                break;
            }
        }

        updateCameraVectors();
    }

    void processMouseMovement(
        GLfloat xoffset, GLfloat yoffset,
        GLboolean constant_pitch = true
    ) {
        xoffset *= m_mouse_sensivity;
        yoffset *= m_mouse_sensivity;

        m_yaw += xoffset;
        m_pitch += yoffset;

        if(constant_pitch) {
            if(m_pitch > 89.0f)
                m_pitch = 89.0f;
            if(m_pitch < -89.0f)
                m_pitch = -89.0f;
        }

        updateCameraVectors();
    }

    void processMouseScroll(GLfloat yoffset) {
        if(m_zoom >= 1.0f && m_zoom <= 45.0f)
            m_zoom -= yoffset;
        
        if(m_zoom < 1.0f)
            m_zoom = 1.0f;

        if(m_zoom > 45.0f)
            m_zoom = 45.0f;
    }
private:
    void updateCameraVectors() {
        glm::vec3 front;
        front.x = cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
        front.y = sin(glm::radians(m_pitch));
        front.z = sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));

        m_front = glm::normalize(front);

        m_right = glm::normalize(glm::cross(m_front, m_world_up));
        m_up = glm::normalize(glm::cross(m_right, m_front));
    }
};