#ifndef CAMERA_H
#define CAMERA_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

enum Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    UP,
    DOWN
};

const float YAW = -90.0f;
const float PITCH = -15.0f;
const float SPEED = 8.0f;
const float SENSITIVITY = 0.1f;
const float ZOOM = 45.0f;

class Camera {
public:
    glm::vec3 Position;
    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp;

    float Yaw;
    float Pitch;
    float Roll;
    float MovementSpeed;
    float MouseSensitivity;
    float Zoom;

    // Orbit parameters
    float OrbitAngle;
    float OrbitRadius;
    float OrbitHeight;
    glm::vec3 OrbitCenter;

    Camera(glm::vec3 position = glm::vec3(0.0f, 15.0f, 40.0f),
        glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
        float yaw = YAW, float pitch = PITCH)
        : Front(glm::vec3(0.0f, 0.0f, -1.0f)),
        MovementSpeed(SPEED),
        MouseSensitivity(SENSITIVITY),
        Zoom(ZOOM),
        Roll(0.0f),
        OrbitAngle(0.0f),
        OrbitRadius(45.0f),
        OrbitHeight(20.0f),
        OrbitCenter(glm::vec3(0.0f, 0.0f, 0.0f))
    {
        Position = position;
        WorldUp = up;
        Yaw = yaw;
        Pitch = pitch;
        updateCameraVectors();
    }

    glm::mat4 GetViewMatrix() {
        glm::mat4 view = glm::lookAt(Position, Position + Front, Up);
        if (Roll != 0.0f) {
            view = glm::rotate(view, glm::radians(Roll), glm::vec3(0.0f, 0.0f, 1.0f));
        }
        return view;
    }

    glm::mat4 GetProjectionMatrix(float aspectRatio, float nearPlane = 0.1f, float farPlane = 500.0f) {
        return glm::perspective(glm::radians(Zoom), aspectRatio, nearPlane, farPlane);
    }

    void ProcessKeyboard(Camera_Movement direction, float deltaTime) {
        float velocity = MovementSpeed * deltaTime;
        if (direction == FORWARD)
            Position += Front * velocity;
        if (direction == BACKWARD)
            Position -= Front * velocity;
        if (direction == LEFT)
            Position -= Right * velocity;
        if (direction == RIGHT)
            Position += Right * velocity;
        if (direction == UP)
            Position += WorldUp * velocity;
        if (direction == DOWN)
            Position -= WorldUp * velocity;
    }

    void ProcessRotation(float pitchOffset, float yawOffset, float rollOffset) {
        Yaw += yawOffset;
        Pitch += pitchOffset;
        Roll += rollOffset;

        if (Pitch > 89.0f) Pitch = 89.0f;
        if (Pitch < -89.0f) Pitch = -89.0f;

        updateCameraVectors();
    }

    void Orbit(float deltaTime) {
        OrbitAngle += deltaTime * 20.0f;
        if (OrbitAngle > 360.0f) OrbitAngle -= 360.0f;

        Position.x = OrbitCenter.x + OrbitRadius * cos(glm::radians(OrbitAngle));
        Position.z = OrbitCenter.z + OrbitRadius * sin(glm::radians(OrbitAngle));
        Position.y = OrbitHeight;

        Front = glm::normalize(OrbitCenter - Position);
        Right = glm::normalize(glm::cross(Front, WorldUp));
        Up = glm::normalize(glm::cross(Right, Front));
    }

    void ResetFromOrbit() {
        updateCameraVectors();
    }

private:
    void updateCameraVectors() {
        glm::vec3 front;
        front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        front.y = sin(glm::radians(Pitch));
        front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        Front = glm::normalize(front);
        Right = glm::normalize(glm::cross(Front, WorldUp));
        Up = glm::normalize(glm::cross(Right, Front));
    }
};

#endif