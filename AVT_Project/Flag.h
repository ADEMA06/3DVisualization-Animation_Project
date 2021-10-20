#pragma once
#include "GameObject.h"
#include "vector"
#include "geometry.h"
#include "vsShaderLib.h"
#include "AVTmathLib.h"

class Flag : public GameObject {
	std::vector<struct MyMesh*> poles;

public:
	Flag(vec3 position1, vec3 position2): GameObject(position1) {
		float amb[] = { 0.2f, 0.15f, 0.1f, 1.0f };
		float red[] = { 1.0f, 0.0f, 0.0f, 1.0f };
		float spec[] = { 0.8f, 0.8f, 0.8f, 1.0f };
		float emissive[] = { 0.0f, 0.0f, 0.0f, 1.0f };

		float height = 8.0f;
		MyMesh* amesh = new MyMesh(createCylinder(height, 0.1f, 10));
		setMesh(amesh, amb, red, red, emissive, 100.0f, 0.0f, position1);
		poles.push_back(amesh);
		amesh = new MyMesh(createCylinder(height, 0.1f, 10));
		setMesh(amesh, amb, red, red, emissive, 100.0f, 0.0f, position2);
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