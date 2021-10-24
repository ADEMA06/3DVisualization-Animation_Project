#pragma once
#include "GameObject.h"
#include "vector"
#include "geometry.h"
#include "vsShaderLib.h"
#include "AVTmathLib.h"

class Flag : public GameObject {
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

		printf("%f %f\n", direction.x, direction.z);

		float height = 8.0f;
		MyMesh* amesh = new MyMesh(createCylinder(height, 0.1f, 10));
		amesh = builder.setMesh(amesh, amb, red, red, emissive, 100.0f, 0.0f, { abs(direction.x) == 1.0f ? getPosition().x : getPosition().x + width / 2, getPosition().y, abs(direction.z) == 1.0f ? getPosition().z : getPosition().z + width / 2 });
		poles.push_back(amesh);
		amesh = new MyMesh(createCylinder(height, 0.1f, 10));
		amesh = builder.setMesh(amesh, amb, red, red, emissive, 100.0f, 0.0f, { abs(direction.x) == 1.0f ? getPosition().x : getPosition().x - width / 2, getPosition().y, abs(direction.z) == 1.0f ? getPosition().z : getPosition().z - width / 2 });
		poles.push_back(amesh);
	}

	void draw(VSShaderLib shader, GLint pvm_uniformId, GLint vm_uniformId, GLint normal_uniformId, GLint lPos_uniformId) {
		for (int i = 0; i < poles.size(); i++) {
			setShaders(shader, poles[i]);
			pushMatrix(MODEL);
			rotate(MODEL, 90.0f, 0.0f, 1.0f, 0.0f);
			mMatrix[MODEL][12] = poles[i]->position.x;
			mMatrix[MODEL][13] = poles[i]->position.y;
			mMatrix[MODEL][14] = poles[i]->position.z;
			drawMesh(poles[i], shader, pvm_uniformId, vm_uniformId, normal_uniformId, lPos_uniformId);
			popMatrix(MODEL);
		}
	}
};