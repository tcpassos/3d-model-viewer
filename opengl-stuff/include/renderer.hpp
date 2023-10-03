#pragma once

#include <glm/glm.hpp>

#include "camera.hpp"
#include "object_3d.hpp"
#include "resource_manager.h"
#include "shader.h"

class Renderer {
public:
    Renderer(glm::vec2 dimensions, Camera& camera) {
        this->screenDimensions = dimensions;
        this->camera = &camera;
        this->textureShader = ResourceManager::loadShader("assets/shaders/default_texture.vs", "assets/shaders/default_texture.fs", nullptr, "defaultTextureShader");
        this->colorShader = ResourceManager::loadShader("assets/shaders/default_color.vs", "assets/shaders/default_color.fs", nullptr, "defaultColorShader");
        this->shader = nullptr;
    }

    void render(Object3D& object) {

        if (object.getTexture() != nullptr) {
            shader = &textureShader;
            shader->use();
            this->shader->setInteger("texBuff", 0);
        } else {
            shader = &colorShader;
            shader->use();
            shader->setVector4f("color", object.getColor());
        }
        
        glm::mat4 projection = glm::perspective(glm::radians(camera->cameraZoom), (float)screenDimensions.x / (float)screenDimensions.y, 0.1f, 100.0f);
        shader->setMatrix4("projection", projection);
        shader->setMatrix4("view", camera->getViewMatrix());
        shader->setMatrix4("model", object.getModelMatrix());

        object.draw();
    }

private:
    Shader textureShader;
    Shader colorShader;
    Shader* shader;
    Camera* camera;
    glm::vec2 screenDimensions;
};
