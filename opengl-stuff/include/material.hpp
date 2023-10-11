#pragma once

#include <glm/glm.hpp>
#include "texture.h"

class Material {
public:
	glm::vec3 ambientColor;
	glm::vec3 diffuseColor;
	glm::vec3 specularColor;
	float shininess;
	float opacity;
	Texture2D texture;

	Material(): ambientColor(glm::vec3(1.0f)), diffuseColor(glm::vec3(1.0f)), specularColor(glm::vec3(1.0f)), shininess(1.0f), opacity(1.0f) { }

};