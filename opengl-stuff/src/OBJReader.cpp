#include "obj/OBJReader.hpp"
#include <iostream>

OBJReader::OBJReader() {
    importer = new Assimp::Importer();
}

OBJReader::~OBJReader() {
    delete importer;
}

bool OBJReader::readModel(const char* filePath) {
    const aiScene* scene = importer->ReadFile(filePath, aiProcess_Triangulate | aiProcess_FlipUVs);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        std::cerr << "Assimp error: " << importer->GetErrorString() << std::endl;
        return false;
    }

    vertex.clear();
    texture.clear();
    index.clear();
    texturePaths.clear();
    
    processNode(scene->mRootNode, scene);

    return true;
}

const std::vector<glm::vec3>& OBJReader::getVertices() const {
    return vertex;
}

const std::vector<glm::vec2>& OBJReader::getTexCoords() const {
    return texture;
}

const std::vector<unsigned int>& OBJReader::getIndices() const {
    return index;
}

const std::vector<std::string>& OBJReader::getTexturePaths() const {
    return texturePaths;
}

void OBJReader::processNode(const aiNode* node, const aiScene* scene) {
    // Process all the meshes in this node
    for (unsigned int i = 0; i < node->mNumMeshes; ++i) {
        const aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        processMesh(mesh, scene);
        processTexture(scene->mMaterials[i]);
    }

    // Recursively process child nodes
    for (unsigned int i = 0; i < node->mNumChildren; ++i) {
        processNode(node->mChildren[i], scene);
    }
}

void OBJReader::processMesh(const aiMesh* mesh, const aiScene* scene) {
    for (unsigned int i = 0; i < mesh->mNumVertices; ++i) {
        glm::vec3 auxVertex;
        auxVertex.x = mesh->mVertices[i].x;
        auxVertex.y = mesh->mVertices[i].y;
        auxVertex.z = mesh->mVertices[i].z;
        vertex.push_back(auxVertex);

        if (mesh->mTextureCoords[0]) {
            glm::vec2 texCoord;
            texCoord.x = mesh->mTextureCoords[0][i].x;
            texCoord.y = mesh->mTextureCoords[0][i].y;
            texture.push_back(texCoord);
        }
    }

    for (unsigned int i = 0; i < mesh->mNumFaces; ++i) {
        const aiFace& face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; ++j) {
            index.push_back(face.mIndices[j]);
        }
    }
}

void OBJReader::processTexture(const aiMaterial* material) {
    // Verifique se o material tem texturas
    for (unsigned int i = 0; i < AI_TEXTURE_TYPE_MAX; ++i) {
        aiTextureType textureType = static_cast<aiTextureType>(i);
        unsigned int textureCount = material->GetTextureCount(textureType);

        for (unsigned int j = 0; j < textureCount; ++j) {
            aiString texturePath;
            if (material->GetTexture(textureType, j, &texturePath) == AI_SUCCESS) {
                // O caminho da textura pode ser relativo, então você pode ajustá-lo como necessário.
                // Por exemplo, você pode substituir "\" por "/" no caminho (para sistemas Windows).
                std::string texturePathStr = texturePath.C_Str();
                std::replace(texturePathStr.begin(), texturePathStr.end(), '\\', '/');

                // Adicione o caminho da textura ao vetor texturePaths
                texturePaths.push_back(texturePathStr);
            }
        }
    }
}