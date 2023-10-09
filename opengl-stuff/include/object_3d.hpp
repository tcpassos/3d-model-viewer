#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "mesh.hpp"
#include "texture.h"

class Object3D {
public:
    glm::vec3 position;
    glm::vec3 scale;
    glm::vec3 origin;
    glm::vec3 rotation;

    Object3D(Mesh &objMesh): mesh(objMesh){
        this->position = glm::vec3(0.0f);
        this->scale = glm::vec3(1.0f);
        this->origin = glm::vec3(0.0f);
        this->rotation = glm::vec3(0.0f);
    }

    ~Object3D() {
        this->mesh.deleteBuffers();
    }

    glm::mat4 getModelMatrix() {
        glm::mat4 model = glm::mat4(1.0f);                                                           // identity
        model = glm::translate(model, this->position - this->origin);                                // position
        model = glm::translate(model, this->origin);                                                 // set origin
        model = glm::rotate(model, rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));                         // rotation x
        model = glm::rotate(model, rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));                         // rotation y
        model = glm::rotate(model, rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));                         // rotation z
        model = glm::translate(model, glm::vec3(-this->origin.x, -this->origin.y, -this->origin.z)); // reset origin
        model = glm::scale(model, this->scale);                                                      // resize
        return model;
    }

    void setPosition(glm::vec3 position) {
        this->position = position;
    }

    void setPosition(float x, float y, float z) {
        this->position.x = x;
        this->position.y = y;
        this->position.z = z;
    }

    void setScale(glm::vec3 scale) {
        this->scale = scale;
    }

    void setScale(float x, float y, float z) {
        this->scale.x = x;
        this->scale.y = y;
        this->scale.z = z;
    }

    void setScale(float scale) {
        this->scale = glm::vec3(scale);
    }

    void setRotation(glm::vec3 rotation) {
        this->rotation = rotation;
    }

    void setRotation(float x, float y, float z) {
        this->rotation.x = x;
        this->rotation.y = y;
        this->rotation.z = z;
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

    bool hasTexture() {
        return mesh.hasTexture();
    }

    void draw() {
        if (hasTexture()) {
            glActiveTexture(GL_TEXTURE0);
            mesh.getTexture().bind();
        }
        mesh.bind();
        glDrawElements(GL_TRIANGLES, mesh.getVertexCount(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }

private:
    Mesh mesh;
};
