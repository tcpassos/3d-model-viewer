#pragma once

#include <glm/glm.hpp>

#include "camera.hpp"
#include "object_3d.hpp"
#include "resource_manager.h"
#include "shader.h"

enum RenderModes_
{
    RenderModes_Normal = 1 << 0,
    RenderModes_Wireframe = 1 << 1,
};

typedef int RenderModes;

class Renderer {
public:
    Renderer(glm::vec2 dimensions, Camera& camera) {
        this->screenDimensions = dimensions;
        this->camera = &camera;
        this->lightPosition = glm::vec3(1.2f, 1.0f, 2.0f);
        this->shader = ResourceManager::loadShader("assets/shaders/default.vs", "assets/shaders/default.fs", nullptr, "defaultShader");
        this->wireframeTexture = ResourceManager::loadTexture(glm::vec4(0.0f, 1.0f, 1.0f, 1.0f), "wireframeTexture");

        shader.use();
        shader.setInteger("texBuff", 0);
        glActiveTexture(GL_TEXTURE0);
    }

    void render(Object3D& object, RenderModes renderModes = RenderModes_Normal) {
        // Calculate projection
        glm::mat4 projection = glm::perspective(glm::radians(camera->cameraZoom), (float)screenDimensions.x / (float)screenDimensions.y, 0.1f, 100.0f);
        // Setup shader
        shader.use();
        shader.setMatrix4("projection", projection);
        shader.setMatrix4("view", camera->getViewMatrix());
        shader.setMatrix4("model", object.getModelMatrix());
        shader.setVector3f("lightPos", lightPosition);
        shader.setVector3f("viewPos", camera->position);
        // Bind mesh attribute array
        object.mesh.bind();
        // Normal render
        if (renderModes & RenderModes_Normal) {
            object.mesh.getTexture().bind();
            glDrawElements(GL_TRIANGLES, object.mesh.getVertexCount(), GL_UNSIGNED_INT, 0);
        }
        // Wireframe render
        if (renderModes & RenderModes_Wireframe) {
            wireframeTexture.bind();
            glDrawElements(GL_LINES, object.mesh.getVertexCount(), GL_UNSIGNED_INT, 0);
        }
        // Unbind
        glBindVertexArray(0);
    }

private:
    Shader shader;
    Camera* camera;
    Texture2D wireframeTexture;
    glm::vec3 lightPosition;
    glm::vec2 screenDimensions;
};
