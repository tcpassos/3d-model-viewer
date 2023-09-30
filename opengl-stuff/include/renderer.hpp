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

        // Pass projection matrix to shader (note that in this case it could change every frame)
        glm::mat4 projection = glm::perspective(glm::radians(camera->cameraZoom), (float)screenDimensions.x / (float)screenDimensions.y, 0.1f, 100.0f);
        shader.setMatrix4("projection", projection);

        // Camera/view transformation
        glm::mat4 view = camera->getViewMatrix();
        shader.setMatrix4("view", view);

        // Calculate the model matrix for each object and pass it to shader before drawing
        glm::mat4 model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
        shader.setMatrix4("model", model);

        // Render object
        object.draw();
    }

private:
    Shader shader;
    Camera* camera;
    glm::vec2 screenDimensions;
};
