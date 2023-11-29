#pragma once

#include <fstream>
#include <iostream>
#include <rapidjson/document.h>
#include <string>
#include <vector>

#include "animation.hpp"
#include "light.hpp"
#include "object_3d.hpp"
#include "object_reader.hpp"
#include "transformable_group.hpp"

class Scene {
public:
	Light light;
	std::vector<Object3D*> objects;
	std::vector<Animation> animations;

	/**
	 * Parses a JSON file and returns a Scene object.
	 *
	 * @param jsonFilePath The path to the JSON file.
	 * @return The Scene object.
	 */
	Scene parse(const char* jsonFilePath) {
		Scene scene;

		rapidjson::Document doc;
		std::ifstream file(jsonFilePath);
		std::string json((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
		doc.Parse(json.c_str());
		
		// If the JSON file is invalid, return an empty scene.
		if (doc.HasParseError()) {
			std::cerr << "Error parsing scene from JSON: " << doc.GetParseError() << std::endl;
			return scene;
		}
		// If the JSON file is empty, return an empty scene.
		if (!doc.HasMember("scene")) {
			std::cerr << "Error parsing scene from JSON: \"scene\" member not found" << std::endl;
			return scene;
		}

		const rapidjson::Value& sceneJson = doc["scene"];

		// Parse objects
		if (sceneJson.HasMember("objects")) {
			const rapidjson::Value& objectsJson = sceneJson["objects"];
			parseObjects(objectsJson);
		}
		// Parse light
		if (sceneJson.HasMember("light")) {
			const rapidjson::Value& lightJson = sceneJson["light"];
			parseLight(lightJson);
		}

		return scene;
	}

private:

	/**
	 * Parses the objects from the JSON file.
	 *
	 * @param objectsJson The JSON object containing the objects.
	 * @param scene The scene to add the objects to.
	 */
	void parseObjects(const rapidjson::Value& objectsJson) {
		ObjectReader objReader;

		for (auto& o : objectsJson.GetArray()) {
			if (!o.HasMember("path")) {
				std::cerr << "Error parsing object from JSON: \"path\" member not found" << std::endl;
				continue;
			}
			// Parse object
			std::string objectFilePath = o["path"].GetString();
			TransformableGroup objGroup;
			for (Object3D* obj : objReader.readModel(objectFilePath.c_str())) {
				if (o.HasMember("initialPosition")) {
					const rapidjson::Value& initialPositionJson = o["initialPosition"];
					obj->position = glm::vec3(initialPositionJson[0].GetFloat(), initialPositionJson[1].GetFloat(), initialPositionJson[2].GetFloat());
				}
				if (o.HasMember("initialRotation")) {
					const rapidjson::Value& initialRotationJson = o["initialRotation"];
					float rotationX = glm::radians(initialRotationJson[0].GetFloat());
					float rotationY = glm::radians(initialRotationJson[1].GetFloat());
					float rotationZ = glm::radians(initialRotationJson[2].GetFloat());
					obj->rotation = glm::vec3(rotationX, rotationY, rotationZ);
				}
				if (o.HasMember("initialScale")) {
					const rapidjson::Value& initialScaleJson = o["initialScale"];
					obj->scale = glm::vec3(initialScaleJson[0].GetFloat(), initialScaleJson[1].GetFloat(), initialScaleJson[2].GetFloat());
				}
				objects.push_back(obj);
				objGroup.add(objects.size() - 1, objects[objects.size() - 1]);
			}
			// Parse animation
			if (o.HasMember("animation")) {
				const rapidjson::Value& animationJson = o["animation"];
				parseAnimation(animationJson, objGroup);
			}
		}
	}

	/**
	 * Parses animations from the JSON file.
	 *
	 * @param animationJson The JSON object containing the animation.
	 * @param scene The scene to add the animation to.
	 */
	void parseAnimation(const rapidjson::Value& animationJson, TransformableGroup objGroup) {
		std::vector<glm::vec3> positions;
		float duration = 1.0f;
		// Parse positions
		if (animationJson.HasMember("positions")) {
			const rapidjson::Value& positionsJson = animationJson["positions"];
			for (auto& p : positionsJson.GetArray()) {
				positions.push_back(glm::vec3(p[0].GetFloat(), p[1].GetFloat(), p[2].GetFloat()));
			}
		}
		// Parse duration
		if (animationJson.HasMember("duration")) {
			duration = animationJson["duration"].GetFloat();
		}
		Animation animation(objGroup, positions, duration);
		animations.push_back(animation);
	}
	
	/**
	 * Parses the light from the JSON file.
	 *
	 * @param lightJson The JSON object containing the light.
	 * @param scene The scene to add the light to.
	 */
	void parseLight(const rapidjson::Value& lightJson) {
		if (lightJson.HasMember("position")) {
			const rapidjson::Value& positionJson = lightJson["position"];
			light.position = glm::vec3(positionJson[0].GetFloat(), positionJson[1].GetFloat(), positionJson[2].GetFloat());
		}
		if (lightJson.HasMember("color")) {
			const rapidjson::Value& colorJson = lightJson["color"];
			light.color = glm::vec3(colorJson[0].GetFloat(), colorJson[1].GetFloat(), colorJson[2].GetFloat());
		}
		if (lightJson.HasMember("ambientStrength")) {
			light.ambientStrength = lightJson["ambientStrength"].GetFloat();
		}
		if (lightJson.HasMember("diffuseStrength")) {
			light.diffuseStrength = lightJson["diffuseStrength"].GetFloat();
		}
		if (lightJson.HasMember("specularStrength")) {
			light.specularStrength = lightJson["specularStrength"].GetFloat();
		}
	}

};