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
	std::vector<glm::vec3> rotations;
	float duration;
	AnimationType type;

	Animation(TransformableGroup &meshGroup, float duration, AnimationType type = AnimationType_Linear) {
		this->meshGroup = meshGroup;
		this->positions = positions;
		this->duration = duration;
		this->type = type;
		this->frameCounter = 0;
		this->lastTime = 0;
	}

	/**
	 * Animates the mesh group.
	 * @param currentTime The current time in seconds.
	*/
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
		if (!positions.empty()) {
			if (type == AnimationType_Linear) {
				meshGroup.position = linear_interpolation(t, positions);
			} else if (type == AnimationType_Bezier) {
				meshGroup.position = bezier_curve(t, positions);
			}
		}
		// Update the rotation of the mesh group
		if (!rotations.empty()) {
			meshGroup.rotation = linear_interpolation(t, rotations);
		}
    	frameCounter++;
    	meshGroup.update();
	}

private:
	TransformableGroup meshGroup;
	int frameCounter;
	float lastTime;

	/**
	 * Calculates the factorial of a number.
	 * @param n The number to calculate the factorial of.
	*/
	double factorial(int n) {
		double fact = 1;
		for(int i = 1; i <= n; i++) {
			fact *= i;
		}
		return fact;
	}

	/**
	 * Calculates the binomial coefficient (n choose k).
	 * @param n The number of elements.
	 * @param k The number of elements to choose.
	*/
	double binomial_coefficient(int n, int k) {
		return factorial(n) / (factorial(k) * factorial(n - k));
	}

    /**
     * Calculates the linear interpolation between two points.
     * @param t The interpolation parameter.
     * @param points The points to interpolate between.
    */
    glm::vec3 linear_interpolation(float t, const std::vector<glm::vec3>& points) {
        int i = (int) (t * (points.size() - 1));
        float local_t = t * (points.size() - 1) - i;
        return points[i] * (1 - local_t) + points[i + 1] * local_t;
    }

    /**
     * Calculates the bezier curve.
     * @param t The interpolation parameter.
     * @param points The points to interpolate between.
    */
    glm::vec3 bezier_curve(float t, const std::vector<glm::vec3>& points) {
        glm::vec3 result = {0, 0, 0};
        int n = points.size() - 1;
        for(int i = 0; i <= n; i++) {
            float b = binomial_coefficient(n, i) * std::pow(t, i) * std::pow(1 - t, n - i);
            result.x += points[i].x * b;
            result.y += points[i].y * b;
            result.z += points[i].z * b;
        }
        return result;
    }

};