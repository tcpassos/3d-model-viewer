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
        this->shader = ResourceManager::loadShader("assets/shaders/default.vs", "assets/shaders/default.fs", nullptr, "defaultShader");
        this->shader.use();
        // Tell opengl for each sampler to which texture unit it belongs to (only has to be done once)
        this->shader.setInteger("texBuff", 0);
    }

    void render(Object3D& object) {
        shader.use();
        
        glm::mat4 projection = glm::perspective(glm::radians(camera->cameraZoom), (float)screenDimensions.x / (float)screenDimensions.y, 0.1f, 100.0f);
        shader.setMatrix4("projection", projection);
        shader.setMatrix4("view", camera->getViewMatrix());
        shader.setMatrix4("model", object.getModelMatrix());

        object.draw();
    }

private:
    Shader shader;
    Camera* camera;
    glm::vec2 screenDimensions;
};
