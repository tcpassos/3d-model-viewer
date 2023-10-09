#pragma once

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <filesystem>
#include <vector>
#include <glm/glm.hpp>

#include "object_3d.hpp"
#include "resource_manager.h"

namespace fs = std::filesystem;

class ObjectReader {
public:
    ObjectReader() {
        importer = new Assimp::Importer();
    }

    ~ObjectReader() {
        delete importer;
    }

    std::vector<Object3D*> readModel(const char* filePath) {
        const aiScene* scene = importer->ReadFile(filePath, aiProcess_Triangulate | aiProcess_FlipUVs);

        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
            std::cerr << "Assimp error: " << importer->GetErrorString() << std::endl;
        }

        std::vector<Mesh> meshes;
        std::vector<Object3D*> objects;
        std::vector<glm::vec3> vertices;
        std::vector<glm::vec2> textureCoords;
        std::vector<GLuint> indices;

        processNode(scene->mRootNode, scene, meshes, filePath);

        for (Mesh& mesh : meshes) {
            Object3D* object = new Object3D(mesh);
            objects.push_back(object);
        }

        return objects;
    }

private:
    Assimp::Importer* importer;

    void processNode(const aiNode* node, const aiScene* scene, std::vector<Mesh>& meshes, const std::string& objPath) {
        // Process all the meshes in this node
        for (unsigned int i = 0; i < node->mNumMeshes; ++i) {
            const aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            meshes.push_back(createMesh(mesh, scene, objPath));
        }
        // Recursively process child nodes
        for (unsigned int i = 0; i < node->mNumChildren; ++i) {
            processNode(node->mChildren[i], scene, meshes, objPath);
        }
    }

    Mesh createMesh(const aiMesh* mesh, const aiScene* scene, const std::string& objPath) {
        std::vector<glm::vec3> vertices;
        std::vector<glm::vec2> textureCoords;
        std::vector<glm::vec3> normals;
        std::vector<GLuint> indices;

        for (unsigned int i = 0; i < mesh->mNumVertices; ++i) {
            glm::vec3 auxVertex;
            auxVertex.x = mesh->mVertices[i].x;
            auxVertex.y = mesh->mVertices[i].y;
            auxVertex.z = mesh->mVertices[i].z;
            vertices.push_back(auxVertex);

            if (mesh->HasTextureCoords(0)) {
                glm::vec2 texCoord;
                texCoord.x = mesh->mTextureCoords[0][i].x;
                texCoord.y = mesh->mTextureCoords[0][i].y;
                textureCoords.push_back(texCoord);
            }

            if (mesh->HasNormals()) {
                glm::vec3 normal;
                normal.x = mesh->mNormals[i].x;
                normal.y = mesh->mNormals[i].y;
                normal.z = mesh->mNormals[i].z;
                normals.push_back(normal);
            }
        }

        for (unsigned int i = 0; i < mesh->mNumFaces; ++i) {
            const aiFace& face = mesh->mFaces[i];
            for (unsigned int j = 0; j < face.mNumIndices; ++j) {
                indices.push_back(face.mIndices[j]);
            }
        }

        // Try to get mesh texture
        if (mesh->mMaterialIndex >= 0 && mesh->mMaterialIndex < scene->mNumMaterials) {
            std::string texturePath = getTextureForMaterial(scene->mMaterials[mesh->mMaterialIndex], objPath);
            if (!texturePath.empty()) {
                std::string meshTexturePath = relativizePath(objPath, texturePath).c_str();
                Texture2D meshTexture = ResourceManager::loadTexture(meshTexturePath.c_str(), texturePath);
                return Mesh(vertices, textureCoords, normals, indices, meshTexture);
            }
        }

        return Mesh(vertices, normals, indices);
    }

    std::string getTextureForMaterial(const aiMaterial* material, const std::string& objPath) {
        for (unsigned int i = 0; i < AI_TEXTURE_TYPE_MAX; ++i) {
            aiTextureType textureType = static_cast<aiTextureType>(i);
            unsigned int textureCount = material->GetTextureCount(textureType);

            for (unsigned int j = 0; j < textureCount; ++j) {
                aiString texturePath;
                if (material->GetTexture(textureType, j, &texturePath) == AI_SUCCESS) {
                    std::string texturePathStr = texturePath.C_Str();
                    return texturePathStr;
                }
            }
        }
        return "";
    }

    std::string relativizePath(const std::string& basePath, const std::string& relativePath) {
        fs::path base(basePath);
        fs::path relative(relativePath);
        if (fs::is_regular_file(base)) {
            return (base.parent_path() / relative).string();
        } else {
            return (base / relative).string();
        }
    }

};