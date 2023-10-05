#pragma once

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <vector>
#include <glm/glm.hpp>

#include "object_3d.hpp"

class ObjectReader {
public:
    ObjectReader() {
        importer = new Assimp::Importer();
    }

    ~ObjectReader() {
        delete importer;
    }

    Object3D readModel(const char* filePath) {
        const aiScene* scene = importer->ReadFile(filePath, aiProcess_Triangulate | aiProcess_FlipUVs);

        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
            std::cerr << "Assimp error: " << importer->GetErrorString() << std::endl;
        }

        std::vector<glm::vec3> vertices;
        std::vector<glm::vec2> textureCoords;
        std::vector<GLuint> indices;

        processNode(scene->mRootNode, scene, vertices, textureCoords, indices);

        Mesh mesh(vertices, indices);
        return Object3D(mesh);
    }

private:
    Assimp::Importer* importer;

    void processNode(const aiNode* node, const aiScene* scene, std::vector<glm::vec3> &vertices, std::vector<glm::vec2> &textureCoords, std::vector<GLuint> &indices) {
        // Process all the meshes in this node
        for (unsigned int i = 0; i < node->mNumMeshes; ++i) {
            const aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            processMesh(mesh, scene, vertices, textureCoords, indices);
            //processTexture(scene->mMaterials[i]);
        }
        // Recursively process child nodes
        for (unsigned int i = 0; i < node->mNumChildren; ++i) {
            processNode(node->mChildren[i], scene, vertices, textureCoords, indices);
        }
    }

    void processMesh(const aiMesh* mesh, const aiScene* scene, std::vector<glm::vec3> &vertices, std::vector<glm::vec2> &textureCoords, std::vector<GLuint> &indices) {
        for (unsigned int i = 0; i < mesh->mNumVertices; ++i) {
            glm::vec3 auxVertex;
            auxVertex.x = mesh->mVertices[i].x;
            auxVertex.y = mesh->mVertices[i].y;
            auxVertex.z = mesh->mVertices[i].z;
            vertices.push_back(auxVertex);

            if (mesh->mTextureCoords[0]) {
                glm::vec2 texCoord;
                texCoord.x = mesh->mTextureCoords[0][i].x;
                texCoord.y = mesh->mTextureCoords[0][i].y;
                textureCoords.push_back(texCoord);
            }
        }

        for (unsigned int i = 0; i < mesh->mNumFaces; ++i) {
            const aiFace& face = mesh->mFaces[i];
            for (unsigned int j = 0; j < face.mNumIndices; ++j) {
                indices.push_back(face.mIndices[j]);
            }
        }
    }

    /*void processTexture(const aiMaterial* material) {
        // Check if material has texture
        for (unsigned int i = 0; i < AI_TEXTURE_TYPE_MAX; ++i) {
            aiTextureType textureType = static_cast<aiTextureType>(i);
            unsigned int textureCount = material->GetTextureCount(textureType);

            for (unsigned int j = 0; j < textureCount; ++j) {
                aiString texturePath;
                if (material->GetTexture(textureType, j, &texturePath) == AI_SUCCESS) {
                    std::string texturePathStr = texturePath.C_Str();
                    std::replace(texturePathStr.begin(), texturePathStr.end(), '\\', '/');
                    texturePaths.push_back(texturePathStr);
                }
            }
        }
    }*/
};