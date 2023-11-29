#pragma once

#include <transformable_group.hpp>
#include <glm/glm.hpp>

#include <vector>
#include <iostream>
#include <map>

#define FRAMES_PER_SECOND 60

enum AnimationType_
{
	AnimationType_Bezier = 1 << 0,
	AnimationType_Linear = 1 << 1,
};
typedef int AnimationType;

class Animation {

public:
	std::vector<glm::vec3> positions;
	float duration;
	AnimationType type;

	Animation(TransformableGroup &meshGroup, std::vector<glm::vec3> positions, float duration, AnimationType type = AnimationType_Linear) {
		this->meshGroup = meshGroup;
		this->positions = positions;
		this->duration = duration;
		this->type = type;
		this->frameCounter = 0;
		this->lastTime = 0;
	}

void animate(float currentTime) {
    // Limit the number of frames per second
    if ((currentTime - lastTime) < (1.0f / FRAMES_PER_SECOND)) {
        return;
    }
    // Update the last time
    lastTime = currentTime;
    // Reset the frame counter if the animation is over
    if (frameCounter >= (duration * FRAMES_PER_SECOND))
        frameCounter = 0;
    // Calculate the t parameter of the bezier curve
	float frameRateDuration = (1.0f / FRAMES_PER_SECOND) / duration;
    float t = frameCounter * frameRateDuration;
	// Update the position of the mesh group
	if (type == AnimationType_Linear) {
		meshGroup.position = linear_interpolation(t);
	} else if (type == AnimationType_Bezier) {
		meshGroup.position = bezier_curve(t);
	}
    frameCounter++;
    meshGroup.update();
}

private:
	TransformableGroup meshGroup;
	int frameCounter;
	float lastTime;

	double factorial(int n) {
		double fact = 1;
		for(int i = 1; i <= n; i++) {
			fact *= i;
		}
		return fact;
	}

	double binomial_coefficient(int n, int k) {
		return factorial(n) / (factorial(k) * factorial(n - k));
	}

	glm::vec3 linear_interpolation(float t) {
		int i = t * (positions.size() - 1);
		float local_t = t * (positions.size() - 1) - i;
		return positions[i] * (1 - local_t) + positions[i + 1] * local_t;
	}

	glm::vec3 bezier_curve(double t) {
		glm::vec3 result = {0, 0, 0};
		int n = positions.size() - 1;
		for(int i = 0; i <= n; i++) {
			double b = binomial_coefficient(n, i) * std::pow(t, i) * std::pow(1 - t, n - i);
			result.x += positions[i].x * b;
			result.y += positions[i].y * b;
			result.z += positions[i].z * b;
		}
		return result;
	}

};