#pragma once

#include <vector>
#include <glad/glad.h>

#include "texture.h"

class Mesh {
public:
    Mesh(const std::vector<glm::vec3>& vertices,
        const std::vector<glm::vec2>& texCoords,
        const std::vector<glm::vec3>& normals,
        const std::vector<GLuint>& indices,
        Texture2D texture,
        std::string name) : Mesh(vertices, normals, indices, name) {

        this->texture = texture;
        this->textureLoaded = true;

        glGenBuffers(1, &TBO);
        glBindVertexArray(VAO);

        // Texture
        glBindBuffer(GL_ARRAY_BUFFER, TBO);
        glBufferData(GL_ARRAY_BUFFER, texCoords.size() * sizeof(glm::vec2), texCoords.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);

        // Unbind
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    Mesh(const std::vector<glm::vec3>& vertices,
        const std::vector<glm::vec3>& normals,
        const std::vector<GLuint>& indices,
        std::string name) {

        vertexCount = static_cast<GLsizei>(indices.size());
        this->textureLoaded = false;
        this->vertices = vertices;
        this->indices = indices;

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &NBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);

        // Vertices
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), vertices.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);
        glEnableVertexAttribArray(0);

        // Normals
        glBindBuffer(GL_ARRAY_BUFFER, NBO);
        glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), normals.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);
        glEnableVertexAttribArray(2);

        // Indexes
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (void*)0);
        glEnableVertexAttribArray(3);

        // Unbind
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    void bind() {
        glBindVertexArray(VAO);
    }

    GLsizei getVertexCount() {
        return this->vertexCount;
    }

    bool hasTexture() {
        return this->textureLoaded;
    }

    Texture2D getTexture() {
        return this->texture;
    }

    std::vector<glm::vec3>& getVertices() {
        return this->vertices;
    }

    std::vector<GLuint>& getIndices() {
        return this->indices;
    }

    std::string getName() {
        return this->name;
    }

    void deleteBuffers() {
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &NBO);
        glDeleteBuffers(1, &TBO);
        glDeleteBuffers(1, &EBO);
        glDeleteVertexArrays(1, &VAO);
    }

private:
    GLuint VAO, VBO, NBO, TBO, EBO;
    GLsizei vertexCount;
    std::vector<glm::vec3> vertices;
    std::vector<GLuint> indices;
    Texture2D texture;
    bool textureLoaded;
    std::string name;
};