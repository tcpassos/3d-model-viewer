#ifndef OBJREADER_H
#define OBJREADER_H

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <vector>
#include <glm/glm.hpp>

class OBJReader {
public:
    OBJReader();
    ~OBJReader();

    bool readModel(const char* filePath);

    const std::vector<glm::vec3>& getVertices() const;
    const std::vector<glm::vec2>& getTexCoords() const;
    const std::vector<unsigned int>& getIndices() const;
    const std::vector<std::string>& getTexturePaths() const;
private:
    Assimp::Importer* importer;
    std::vector<glm::vec3> vertex;
    std::vector<glm::vec2> texture;
    std::vector<unsigned int> index;
    std::vector<std::string> texturePaths;

    void processNode(const aiNode* node, const aiScene* scene);
    void processMesh(const aiMesh* mesh, const aiScene* scene);
    void processTexture(const aiMaterial* material);
};

#endif