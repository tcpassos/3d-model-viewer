#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

class Transformable {
public:
    glm::vec3 position;
    glm::vec3 scale;
    glm::vec3 origin;
    glm::vec3 rotation;

	Transformable() {
        this->position = glm::vec3(0.0f);
        this->scale = glm::vec3(1.0f);
        this->origin = glm::vec3(0.0f);
        this->rotation = glm::vec3(0.0f);
	}

    void move(float x, float y, float z) {
        moveX(x); moveY(y), moveZ(z);
    }

    void moveX(float movement) {
        this->position.x += movement;
    }

    void moveY(float movement) {
        this->position.y += movement;
    }

    void moveZ(float movement) {
        this->position.z += movement;
    }

    void rotate(float x, float y, float z) {
        rotateX(x); rotateY(y); rotateZ(z);
    }

    void rotateX(float rotation) {
        this->rotation.x += rotation;
    }

    void rotateY(float rotation) {
        this->rotation.y += rotation;
    }

    void rotateZ(float rotation) {
        this->rotation.z += rotation;
    }
};