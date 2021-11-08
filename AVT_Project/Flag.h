#pragma once
#include "StaticObject.h"
#include "vector"
#include "MeshBuilder.h"


class Flag : public StaticObject {
	std::vector<struct MyMesh*> poles;
	float width;
	vec3 direction;

public:
	Flag(vec3 position, float width, vec3 direction): StaticObject(position) {
		this->width = width;
		this->direction = direction;
	}

	void createFlag() {
		MeshBuilder builder;
		float amb[] = { 0.2f, 0.15f, 0.1f, 1.0f };
		float red[] = { 1.0f, 0.0f, 0.0f, 1.0f };
		float spec[] = { 0.8f, 0.8f, 0.8f, 1.0f };
		float emissive[] = { 0.0f, 0.0f, 0.0f, 1.0f };

		float height = 8.0f;
		MyMesh* amesh = new MyMesh(createCylinder(height, 0.1f, 10));
		amesh = builder.setMesh(amesh, amb, red, red, emissive, 100.0f, 0.0f, { abs(direction.x) == 1.0f ? getPosition().x : getPosition().x + width / 2, getPosition().y, abs(direction.z) == 1.0f ? getPosition().z : getPosition().z + width / 2 });
		poles.push_back(amesh);
		amesh = new MyMesh(createCylinder(height, 0.1f, 10));
		amesh = builder.setMesh(amesh, amb, red, red, emissive, 100.0f, 0.0f, { abs(direction.x) == 1.0f ? getPosition().x : getPosition().x - width / 2, getPosition().y, abs(direction.z) == 1.0f ? getPosition().z : getPosition().z - width / 2 });
		poles.push_back(amesh);
	}

	bool betweenPoles(vec3 car_pos) {
		if (direction.x == 1) {
			if (car_pos.x >= poles[0]->position.x - 0.3f && car_pos.x <= poles[0]->position.x + 0.3f && car_pos.z < poles[0]->position.z && car_pos.z > poles[1]->position.z) {
				return true;
			}
		}
		else {
			if (car_pos.z >= poles[0]->position.z - 0.3f && car_pos.z <= poles[0]->position.z + 0.3f && car_pos.x < poles[0]->position.x && car_pos.x > poles[1]->position.x) {
				return true;
			}
		}
		return false;
	}
	
	void draw(VSShaderLib *shader) {
		MeshBuilder builder;
		for (int i = 0; i < poles.size(); i++) {
			builder.setShaders(shader, poles[i]);
			pushMatrix(MODEL);
			rotate(MODEL, 90.0f, 0.0f, 1.0f, 0.0f);
			mMatrix[MODEL][12] = poles[i]->position.x;
			mMatrix[MODEL][13] = poles[i]->position.y;
			mMatrix[MODEL][14] = poles[i]->position.z;
			builder.drawMesh(poles[i], shader);
			popMatrix(MODEL);
		}
	}
};