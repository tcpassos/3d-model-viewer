#pragma once

#include <glm/glm.hpp>

#include "camera.hpp"
#include "light.hpp"
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
    Camera* camera;
    Light* light;

    Renderer(glm::vec2 dimensions, Camera& camera, Light& light) {
        this->screenDimensions = dimensions;
        this->camera = &camera;
        this->light = &light;
        this->shader = ResourceManager::loadShader("assets/shaders/default.vs", "assets/shaders/default.fs", nullptr, "defaultShader");
        this->wireframeTexture = ResourceManager::loadTexture(glm::vec4(0.0f, 1.0f, 1.0f, 1.0f), "wireframeTexture");
        this->defaultTexture = ResourceManager::loadTexture(glm::vec4(0.7f, 0.7f, 0.7f, 1.0f), "defaultTexture");

        shader.use();
        shader.setInteger("texBuff", 0);
        glActiveTexture(GL_TEXTURE0);
    }

    void render(Object3D& object, RenderModes renderModes = RenderModes_Normal) {
        // Calculate projection
        glm::mat4 projection = glm::perspective(glm::radians(camera->cameraZoom), (float)screenDimensions.x / (float)screenDimensions.y, 0.1f, 500.0f);
        // Calculate light position in world space
        // Necessary to calculate light position in world space because the object is translated and rotated
        glm::mat4 model = object.getModelMatrix();
        glm::mat4 inverseModel = glm::inverse(model);
        glm::vec4 lightPositionWorldSpace = inverseModel * glm::vec4(light->position, 1.0);
        // Setup shader
        shader.use();
        shader.setMatrix4("projection", projection);
        shader.setMatrix4("view", camera->getViewMatrix());
        shader.setMatrix4("model", model);
        shader.setVector3f("light.position", glm::vec3(lightPositionWorldSpace));
        shader.setVector3f("light.ambient", light->color * light->ambientStrength);
        shader.setVector3f("light.diffuse", light->color * light->diffuseStrength);
        shader.setVector3f("light.specular", light->color * light->specularStrength);
        shader.setVector3f("viewPos", camera->position);

        Material material = object.mesh.getMaterial();
        shader.setVector3f("material.ambient", material.ambientColor);
        shader.setVector3f("material.diffuse", material.diffuseColor);
        shader.setVector3f("material.specular", material.specularColor);
        shader.setVector3f("material.emissive", material.emissiveColor);
        shader.setFloat("material.shininess", material.shininess);
        shader.setFloat("material.opacity", material.opacity);

        // Bind mesh attribute array
        object.mesh.bind();
        // Normal render
        if (renderModes & RenderModes_Normal) {
            // If has texture
            if (material.texture.id != 0) {
                material.texture.bind();
            } else {
                defaultTexture.bind();
            }
            // Config blending
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glDrawElements(GL_TRIANGLES, object.mesh.getVertexCount(), GL_UNSIGNED_INT, 0);
        }
        // Wireframe render
        if (renderModes & RenderModes_Wireframe) {
            wireframeTexture.bind();
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            glDrawElements(GL_TRIANGLES, object.mesh.getVertexCount(), GL_UNSIGNED_INT, 0);
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }
        // Unbind
        glBindVertexArray(0);
    }

private:
    Shader shader;
    Texture2D wireframeTexture;
    Texture2D defaultTexture;
    glm::vec2 screenDimensions;
};
