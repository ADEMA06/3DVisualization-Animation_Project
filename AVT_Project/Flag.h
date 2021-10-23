#pragma once
#include "StaticObject.h"
#include "vector"
#include "MeshBuilder.h"


class Flag : public StaticObject {
	std::vector<struct MyMesh*> poles;
	float width;

public:
	Flag(vec3 position, float width): StaticObject(position) {
		this->width = width;
	}

	void createFlag() {
		MeshBuilder builder;
		float amb[] = { 0.2f, 0.15f, 0.1f, 1.0f };
		float red[] = { 1.0f, 0.0f, 0.0f, 1.0f };
		float spec[] = { 0.8f, 0.8f, 0.8f, 1.0f };
		float emissive[] = { 0.0f, 0.0f, 0.0f, 1.0f };

		float height = 8.0f;
		MyMesh* amesh = new MyMesh(createCylinder(height, 0.1f, 10));
		amesh = builder.setMesh(amesh, amb, red, red, emissive, 100.0f, 0.0f, { getPosition().x + width / 2, getPosition().y, getPosition().z });
		poles.push_back(amesh);
		amesh = new MyMesh(createCylinder(height, 0.1f, 10));
		amesh = builder.setMesh(amesh, amb, red, red, emissive, 100.0f, 0.0f, { getPosition().x - width / 2, getPosition().y, getPosition().z });
		poles.push_back(amesh);
	}
	

	void draw(VSShaderLib shader) {
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