#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "mesh.hpp"
#include "texture.h"

class Object3D {
public:
    Object3D(Mesh &objMesh, Texture2D &objTexture): mesh(&objMesh), texture(&objTexture) { }

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
