#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "mesh.hpp"
#include "texture.h"
#include "transformable.hpp"

class Object3D: public Transformable {
public:
    Mesh mesh;

    Object3D(Mesh &objMesh): Transformable(), mesh(objMesh) { }

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
};
