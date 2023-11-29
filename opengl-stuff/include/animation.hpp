#pragma once

#include <bezier.hpp>
#include <transformable_group.hpp>
#include <glm/glm.hpp>

#include <vector>
#include <iostream>
#include <map>

#define FRAMES_PER_SECOND 60

class Animation {

public:
	float duration;
	std::vector<glm::vec3> positions;

	Animation(TransformableGroup &meshGroup, std::vector<glm::vec3> positions, float duration) {
		this->meshGroup = meshGroup;
		this->positions = positions;
		this->duration = duration;
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
    meshGroup.position = bezier_curve(t);

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