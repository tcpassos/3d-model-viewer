#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <map>

#include "object_3d.hpp"
#include "transformable.hpp"

class TransformableGroup: public Transformable {
public:
    TransformableGroup() : Transformable() {
        previousPosition = this->position;
        previousRotation = this->rotation;
        previousScale = this->scale;
    }
    
    void add(int id, Transformable* transformable) {
        transformables[id] = transformable;
        updateAttributes();
    }

    void remove(int id) {
        transformables.erase(id);
        updateAttributes();
    }

    void clear() {
        transformables.clear();
        updateAttributes();
    }

    bool contains(int id) {
        return transformables.find(id) != transformables.end();
    }

    size_t size() {
        return transformables.size();
    }

    bool empty() {
        return transformables.empty();
    }

    void update() {
        glm::vec3 deltaPosition = previousPosition - this->position;
        glm::vec3 deltaRotation = previousRotation - this->rotation;
        glm::vec3 deltaScale = previousScale - this->scale;

        for (const auto& [id, transformable] : transformables) {
            transformable->position -= deltaPosition;
            transformable->rotation -= deltaRotation;
            transformable->scale -= deltaScale;
        }

        previousPosition = this->position;
        previousRotation = this->rotation;
        previousScale = this->scale;
    }

private:
    std::map<int, Transformable*> transformables;

    glm::vec3 previousPosition;
    glm::vec3 previousRotation;
    glm::vec3 previousScale;

    void updateAttributes() {
        if (transformables.size() == 1) {
            Transformable* firstElement = transformables.begin()->second;
            this->position = firstElement->position;
            this->rotation = firstElement->rotation;
            this->scale = firstElement->scale;
        } else {
            this->position = glm::vec3(0.0f);
            this->rotation = glm::vec3(0.0f);
            this->scale = glm::vec3(1.0f);
        }
        previousPosition = this->position;
        previousRotation = this->rotation;
        previousScale = this->scale;
    }
};
