#include "camera.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

FreeCamera::FreeCamera(const glm::vec3& pos, const glm::vec3& front, const glm::vec3& up, float yaw, float pitch, float camSpeed, float mouseSensi)
    : m_pos { pos }
    , m_front { front }
    , m_worldUp { up }
    , m_camSpeed { camSpeed }
    , m_mouseSensi { mouseSensi }
    , m_zoom { 45.0f }
    , m_yaw { yaw }          
    , m_pitch { pitch }  
    , m_firstMouse(true)
{
    updateCameraVectors();
}

glm::mat4 FreeCamera::viewMatrix() {
    return glm::lookAt(m_pos, m_pos + m_front, m_up);
}

void FreeCamera::processKeyboardInput(CameraDirection direction, float deltaTime) {
    float camSpeed { m_camSpeed * deltaTime };

    if (direction == FORWARD)
        m_pos += m_front * camSpeed;
    if (direction == BACKWARD)
        m_pos -= m_front * camSpeed;
    if (direction == RIGHT)  
        m_pos += m_right * camSpeed;
    if (direction == LEFT)
        m_pos -= m_right * camSpeed;
}

void FreeCamera::processMouseInput(float xposIn, float yposIn, bool stopRoll) {
   
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (m_firstMouse) {
        m_lastX = xpos;
        m_lastY = ypos;
        
        m_firstMouse = false;

        return;
    }

    float xOffset = xpos - m_lastX;
    float yOffset = m_lastY - ypos; // reversed since y-coordinates go from bottom to top
    
    m_lastX = xpos;
    m_lastY = ypos;

    xOffset *= m_mouseSensi;
    yOffset *= m_mouseSensi;

    m_yaw += xOffset;
    m_pitch += yOffset;

    if (stopRoll) {
        if (m_pitch > 89.0f)
            m_pitch = 89.0f;
        if (m_pitch < -89.0f)
            m_pitch = -89.0f;
    }   

    updateCameraVectors();
}

void FreeCamera::processScrollInput(float yOffset) {
    m_zoom -= (float) yOffset;

    if (m_zoom < 1.0f)
        m_zoom = 1.0f;
    if (m_zoom > 45.0f)
        m_zoom = 45.0f;
}

void FreeCamera::updateCameraVectors() {
    m_front.x = cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
    m_front.y = sin(glm::radians(m_pitch));
    m_front.z = sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
    
    m_front = glm::normalize(m_front);

    m_right = glm::normalize(glm::cross(m_front, m_worldUp));
    m_up = glm::normalize(glm::cross(m_right, m_front));
}
