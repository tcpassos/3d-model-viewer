#pragma once

#include <mesh.hpp>
#include <object_3d.hpp>
#include <resource_manager.h>
#include <texture.h>
#include <obj/OBJReader.hpp>

class Object: public Object3D {
public:
    Object(): objectMesh(objectVertices, objectIndices), Object3D(objectMesh) {
        OBJReader objReader;
        
        bool modelLoaded = objReader.readModel("assets/obj/cube.obj");

        if (modelLoaded) {
            objectVertices = objReader.getVertices();
            objectTexCoords = objReader.getTexCoords();
            objectIndices = objReader.getIndices();

            // Carregue as texturas usando os caminhos fornecidos pelo ObjLoader
            std::vector<std::string> texturePaths = objReader.getTexturePaths();
            if (!texturePaths.empty()) {
                objectTexture = ResourceManager::loadTexture("assets/img/square.png", "objectTexture");
            }
        }
    }

private:
    std::vector<glm::vec3> objectVertices = { };

    std::vector<glm::vec2> objectTexCoords = { };

    std::vector<unsigned int> objectIndices = { };

    Mesh objectMesh;
    Texture2D objectTexture;
};