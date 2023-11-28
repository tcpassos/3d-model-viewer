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

		this->segmentCount = int(timePoints[timePoints.size() - 1]*60);

		frameCounter = 0;
		lastframe = 0;
	}

void animate(float frame) {
	if ((frame - lastframe) > (1.0f / 60.0f))
	{
		lastframe = frame;
		

		if (isLoopAnimation)
			animateLooped();
		else
			animateSimple();
		
		frameCounter++;

		update();
	}
}

void animateLooped(){

	if (frameCounter >= (segmentCount * 2))
		frameCounter = 0;

	int tamPositions = positions.size();
	float currentTime = frameCounter * (1.0f / 60.0f);
	
	int x = 0;
	float t = 0.f;

	for(x = 0; x < timePoints.size() - 1; x++){
		if(currentTime >= timePoints[x] && currentTime < timePoints[x+1]){
			break;
		}
	}

	for (int y = 0; y < tamPositions; y += 3)
	{
		if(x >= y && x < y+4)
		{
			if (frameCounter < (timePoints[y] * 60))
				continue;
			if (frameCounter > (segmentCount*2) - (timePoints[y] * 60))
				continue;

			if (tamPositions - y >= 4) {

				if (frameCounter < segmentCount) {
					t = bezierpp::invLerp<float>(float(timePoints[y] * 60), float(timePoints[y + 3] * 60), float(frameCounter % segmentCount));
				}
				else {
					t = bezierpp::invLerp<float>(float(timePoints[y] * 60), float(timePoints[y + 3] * 60), float(segmentCount - (frameCounter % segmentCount)));
				}
				meshGroup.position = bezierpp::cubicBezierCurve(positions[y], positions[y + 1], positions[y + 2], positions[y + 3], t);
			}
			else if (tamPositions - y == 3) {
				if (frameCounter < segmentCount) {
					t = bezierpp::invLerp<float>(float(timePoints[y] * 60), float(timePoints[y + 2] * 60), float(frameCounter % segmentCount));
				}
				else {
					t = bezierpp::invLerp<float>(float(timePoints[y] * 60), float(timePoints[y + 2] * 60), float(segmentCount - (frameCounter % segmentCount)));
				}
				meshGroup.position = bezierpp::quadraticBezierCurve(positions[y], positions[y + 1], positions[y + 2], t);
			}
			else if (tamPositions - y == 2) {
				if (frameCounter < segmentCount) {
					t = bezierpp::invLerp<float>(float(timePoints[y] * 60), float(timePoints[y + 1] * 60), float(frameCounter % segmentCount));
				}
				else {
					t = bezierpp::invLerp<float>(float(timePoints[y] * 60), float(timePoints[y + 1] * 60), float(segmentCount - (frameCounter % segmentCount)));
				}
				meshGroup.position = bezierpp::linearBezierCurve(positions[y], positions[y + 1], t);
			}
			else
				return;
		}
	}

}

void animateSimple(){

	if (frameCounter >= segmentCount)
		frameCounter = 0;

	int tamPositions = positions.size();
	float currentTime = frameCounter * (1.0f / 60.0f);

	int x = 0;
	float t = 0.f;

	for (x = 0; x < timePoints.size() - 1; x++) {
		if (currentTime >= timePoints[x] && currentTime < timePoints[x + 1]) {
			break;
		}
	}

	for (int y = 0; y < tamPositions; y += 3)
	{
		if (x >= y && x < y + 4)
		{

			if (frameCounter < (timePoints[y] * 60))
				continue;

			if (tamPositions - y >= 4) {
				t = bezierpp::invLerp<float>(float(timePoints[y] * 60), float(timePoints[y + 3] * 60), float(frameCounter % segmentCount));
				meshGroup.position = bezierpp::cubicBezierCurve(positions[y], positions[y + 1], positions[y + 2], positions[y + 3], t);
			}
			else if (tamPositions - y == 3) {
				t = bezierpp::invLerp<float>(float(timePoints[y] * 60), float(timePoints[y + 2] * 60), float(frameCounter % segmentCount));
				meshGroup.position = bezierpp::quadraticBezierCurve(positions[y], positions[y + 1], positions[y + 2], t);
			}
			else if (tamPositions - y == 2) {
				t = bezierpp::invLerp<float>(float(timePoints[y] * 60), float(timePoints[y + 1] * 60), float(frameCounter % segmentCount));
				meshGroup.position = bezierpp::linearBezierCurve(positions[y], positions[y + 1], t);
			}
			else
				return;
		}
	}
}

void update() {
	meshGroup.update();
}

};