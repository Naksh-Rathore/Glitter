#pragma once

#include <glm/glm.hpp>

enum CameraDirection {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    NONE
};  

class FreeCamera {
    private:
        glm::vec3 m_pos{};
        glm::vec3 m_front{};
        glm::vec3 m_up{};
        glm::vec3 m_direction{};
        glm::vec3 m_right{};
        glm::vec3 m_worldUp{};

        float m_yaw{};
        float m_pitch{};

        float m_lastX;
        float m_lastY;

        bool m_firstMouse;

        void updateCameraVectors();

    public:
        FreeCamera(const glm::vec3& pos, const glm::vec3& front, const glm::vec3& up, float yaw, float pitch, float camSpeed, float mouseSensi);

        glm::mat4 viewMatrix();

        void processKeyboardInput(CameraDirection direction, float deltaTime);
        void processMouseInput(float xposIn, float yposIn, bool stopRoll = true);
        void processScrollInput(float yOffset);

        float m_camSpeed;
        float m_mouseSensi;
        float m_zoom;
};
    
