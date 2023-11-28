#pragma once

#include <bezier.hpp>
#include <transformable_group.hpp>
#include <glm/glm.hpp>

#include <vector>
#include <iostream>
#include <map>

class Animation {
private:
	TransformableGroup meshGroup;
	std::vector<float> timePoints;
	std::vector<glm::vec3> positions;

	bool isLoopAnimation = false;

	int frameCounter;
	int segmentCount;
	float lastframe;

public:
	Animation(TransformableGroup &meshGroup, std::vector<float> timePoints, std::vector<glm::vec3> positions, bool isLoopAnimation = false) {
		this->timePoints = timePoints;
		this->positions = positions;
		this->meshGroup = meshGroup;
		this->isLoopAnimation = isLoopAnimation;

		this->segmentCount = int((timePoints[timePoints.size() - 1] - timePoints[0])*60);

		frameCounter = 0;
		lastframe = 0;
	}

void animate(float frame) {
	if (frame - lastframe > 1.0f / 60.0f)
	{
		lastframe = frame;

		if (isLoopAnimation)
			animateLooped(frame);
		else
			animateSimple(frame);

		frameCounter++;
		update();
	}
}

void animateLooped(float frame){
	if (frameCounter > segmentCount * 2)
		frameCounter = 0;
	
	float t = 0.f;

	if (frameCounter < segmentCount) {
		t = bezierpp::invLerp<float>(0.f, float(segmentCount - 1), float(frameCounter % segmentCount));
	}
	else {
		t = bezierpp::invLerp<float>(0.f, float(segmentCount - 1), float(segmentCount - (frameCounter % segmentCount)));
	}

	int tamPositions = positions.size();

	for (int x = 0; x < tamPositions; x += 4)
	{
		if (tamPositions - x >= 4) {
			meshGroup.position = bezierpp::cubicBezierCurve(positions[x], positions[x + 1], positions[x + 2], positions[x + 3], t);
		}else if (tamPositions - x == 3) {
			meshGroup.position = bezierpp::quadraticBezierCurve(positions[x], positions[x + 1], positions[x + 2], t);
		}
		else if (tamPositions - x == 2) {
			meshGroup.position = bezierpp::linearBezierCurve(positions[x], positions[x + 1], t);
		}
		else
			return;
	}

}

void animateSimple(float frame){
	if (frameCounter > segmentCount)
		frameCounter = 0;

	float t = bezierpp::invLerp<float>(0.f, float(segmentCount - 1), float(frameCounter % segmentCount));

	int tamPositions = positions.size();

	for (int x = 0; x < tamPositions; x += 4)
	{
		if (tamPositions - x >= 4) {
			meshGroup.position = bezierpp::cubicBezierCurve(positions[x], positions[x + 1], positions[x + 2], positions[x + 3], t);
		}
		else if (tamPositions - x == 3) {
			meshGroup.position = bezierpp::quadraticBezierCurve(positions[x], positions[x + 1], positions[x + 2], t);
		}
		else if (tamPositions - x == 2) {
			meshGroup.position = bezierpp::linearBezierCurve(positions[x], positions[x + 1], t);
		}
		else
			return;
	}
}

void update() {
	meshGroup.update();
}

};