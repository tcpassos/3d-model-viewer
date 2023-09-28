#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "mesh.hpp"
#include "texture.h"

class Object3D {
public:
    Object3D(Mesh* mesh, Texture2D* texture) {
        this->mesh = mesh;
        this->texture = texture;
    }

    void draw();

private:
    Mesh* mesh;
    Texture2D* texture;
};

void Object3D::draw() {
    glActiveTexture(GL_TEXTURE0);
    texture->bind();
    mesh->draw();
}
