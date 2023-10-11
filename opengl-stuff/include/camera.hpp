#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

// Default camera values
const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 3.0f;
const float SENSITIVITY = 0.1f;
const float ZOOM = 45.0f;

// An abstract camera class that processes input and calculates the corresponding Euler Angles, Vectors and Matrices for use in OpenGL
class Camera {
public:
    // camera Attributes
    glm::vec3 position;
    glm::vec3 front;
    glm::vec3 up;
    glm::vec3 right;
    glm::vec3 worldUp;
    // euler Angles
    float yaw;
    float pitch;
    // camera options
    float movementSpeed;
    float mouseSensitivity;
    float cameraZoom;

    // Constructor with vectors
    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH) : front(glm::vec3(0.0f, 0.0f, -1.0f)), movementSpeed(SPEED), mouseSensitivity(SENSITIVITY), cameraZoom(ZOOM) {
        this->position = position;
        this->worldUp = up;
        this->yaw = yaw;
        this->pitch = pitch;
        updateCameraVectors();
    }

    // Constructor with scalar values
    Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch) : front(glm::vec3(0.0f, 0.0f, -1.0f)), movementSpeed(SPEED), mouseSensitivity(SENSITIVITY), cameraZoom(ZOOM) {
        position = glm::vec3(posX, posY, posZ);
        worldUp = glm::vec3(upX, upY, upZ);
        yaw = yaw;
        pitch = pitch;
        updateCameraVectors();
    }

    // Returns the view matrix calculated using Euler Angles and the LookAt Matrix
    glm::mat4 getViewMatrix() {
        return glm::lookAt(position, position + front, up);
    }

    void moveForward(float deltaTime) {
        float velocity = movementSpeed * deltaTime;
        position += front * velocity;
    }

    void moveBackward(float deltaTime) {
        float velocity = movementSpeed * deltaTime;
        position -= front * velocity;
    }

    void moveLeft(float deltaTime) {
        float velocity = movementSpeed * deltaTime;
        position -= right * velocity;
    }

    void moveRight(float deltaTime) {
        float velocity = movementSpeed * deltaTime;
        position += right * velocity;
    }

    void moveUp(float deltaTime) {
        float velocity = movementSpeed * deltaTime;
        position += up * velocity;
    }

    void moveDown(float deltaTime) {
        float velocity = movementSpeed * deltaTime;
        position -= up * velocity;
    }

    // Processes input received from a mouse input system. Expects the offset value in both the x and y direction.
    void processMouseMovement(float xoffset, float yoffset) {
        yaw += xoffset * mouseSensitivity;
        pitch += yoffset * mouseSensitivity;
        // Update Front, Right and Up Vectors using the updated Euler angles
        updateCameraVectors();
    }

    // Processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
    void zoom(float yoffset) {
        cameraZoom -= (float)yoffset;
        if (cameraZoom < 1.0f)
            cameraZoom = 1.0f;
        if (cameraZoom > 45.0f)
            cameraZoom = 45.0f;
    }

    // Calculates the front vector from the Camera's (updated) Euler Angles
    void updateCameraVectors() {
        // make sure that when pitch is out of bounds, screen doesn't get flipped
        if (pitch > 89.0f)
            pitch = 89.0f;
        if (pitch < -89.0f)
            pitch = -89.0f;
        // calculate the new Front vector
        glm::vec3 newFront;
        newFront.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        newFront.y = sin(glm::radians(pitch));
        newFront.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        front = glm::normalize(newFront);
        // also re-calculate the Right and Up vector
        right = glm::normalize(glm::cross(front, worldUp));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
        up = glm::normalize(glm::cross(right, front));
    }

};