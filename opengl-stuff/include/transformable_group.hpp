#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <map>

#include "object_3d.hpp"
#include "transformable.hpp"

class TransformableGroup: public Transformable {
public:
    TransformableGroup() : Transformable() { }
    
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
        if (transformables.size() == 1) {
            Transformable* firstElement = transformables.begin()->second;
            firstElement->position = this->position;
            firstElement->rotation = this->rotation;
            firstElement->scale = this->scale;
        } else {
            for (const auto& [id, transformable] : transformables) {
                transformable->position += this->position;
                transformable->rotation += this->rotation;
                transformable->scale += glm::vec3(this->scale.x - 1.0f, this->scale.y - 1.0f, this->scale.z - 1.0f);
            }
            updateAttributes();
        }
    }

private:
    std::map<int, Transformable*> transformables;

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
    }
};
