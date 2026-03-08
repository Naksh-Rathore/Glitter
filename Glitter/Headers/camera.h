#pragma once

#include <glm/glm.hpp>

enum CameraDirection {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    NONE
};  

struct FreeCameraSettings {
    glm::vec3 m_pos{};

    float m_camSpeed;
    float m_mouseSensi;
    float m_zoom;

    FreeCameraSettings(const glm::vec3& pos, const float camSpeed, float mouseSensi, float zoom);
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
        FreeCamera(FreeCameraSettings settings);
        FreeCamera(const glm::vec3& pos, const float camSpeed, float mouseSensi, float zoom);

        glm::mat4 viewMatrix();

        void processKeyboardInput(CameraDirection direction, float deltaTime);
        void processMouseInput(float xposIn, float yposIn, bool stopRoll = true);
        void processScrollInput(float yOffset);

        glm::vec3 pos() { return m_pos; }

        float m_camSpeed;
        float m_mouseSensi;
        float m_zoom;
};
    
