#pragma once

#include <glm/glm.hpp>

class Light {
public:
	glm::vec3 position;
	glm::vec3 color;

	Light(glm::vec3 lightPosition = glm::vec3(1.2f, 2.0f, 4.0f), glm::vec3 lightColor = glm::vec3(1.0f)):
		position(lightPosition), color(lightColor) { }

	glm::vec3 getAmbientColor() {
		return getDiffuseColor() * glm::vec3(0.2f);
	}

	glm::vec3 getDiffuseColor() {
		return color * glm::vec3(0.5f);
	}

	glm::vec3 getSpecularColor() {
		return glm::vec3(1.0f);
	}
};