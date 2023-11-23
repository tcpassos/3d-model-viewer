#pragma once

#include <glm/glm.hpp>

class Light {
public:
	glm::vec3 position;
	glm::vec3 color;
	float ambientStrength;
	float specularStrength;

	Light() : position(glm::vec3(1.2f, 2.0f, 4.0f)),
		color(glm::vec3(1.0f, 1.0f, 1.0f)),
		ambientStrength(0.3f),
		specularStrength(0.5f) {}
};